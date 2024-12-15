// [[Rcpp::plugins(cpp11)]]

#include <Rcpp.h>
#include "ripser.h" // Include ripser functionality
#include <vector>
#include <string>

using namespace Rcpp;

// Reads point cloud data from R and creates a Euclidean distance matrix
ripser::euclidean_distance_matrix read_point_cloud_from_R(const NumericMatrix& coordinates) {
  // Convert NumericMatrix to std::vector<std::vector<double>> for Ripser
  std::vector<std::vector<double>> points(coordinates.nrow(), std::vector<double>(coordinates.ncol()));
  for (int i = 0; i < coordinates.nrow(); i++) {
    for (int j = 0; j < coordinates.ncol(); j++) {
      points[i][j] = coordinates(i, j);
    }
  }
  return ripser::euclidean_distance_matrix(std::move(points));
}

// Reads a distance matrix from R and creates a compressed lower distance matrix
ripser::compressed_lower_distance_matrix read_distance_matrix_from_R(const NumericMatrix& distance_matrix_R) {
  // Check if the distance matrix is square
  int n = distance_matrix_R.nrow();
  if (distance_matrix_R.ncol() != n) {
    Rcpp::stop("Distance matrix must be square");
  }
  
  // Extract lower triangular part of the distance matrix
  std::vector<ripser::value_t> distances;
  for (int i = 1; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      distances.push_back(distance_matrix_R(i, j));
    }
  }
  return ripser::compressed_lower_distance_matrix(std::move(distances));
}

// Converts computed barcodes into R DataFrame
DataFrame convert_barcodes_to_R(const std::vector<std::vector<ripser::interval>>& barcodes) {
  // Vectors to hold dimensions, birth times, and death times
  std::vector<int> dimensions;
  std::vector<double> births;
  std::vector<double> deaths;
  
  // Populate vectors with barcode data
  for (size_t dim = 0; dim < barcodes.size(); ++dim) {
    for (const auto& interval : barcodes[dim]) {
      dimensions.push_back(dim);
      births.push_back(interval.birth);
      deaths.push_back(interval.death);
    }
  }
  
  // Create DataFrame to return to R
  return DataFrame::create(
    Named("dimension") = dimensions,
    Named("birth") = births,
    Named("death") = deaths
  );
}

// [[Rcpp::export]]
DataFrame ripser_cpp(const NumericMatrix& input_data,
                     int dim_max = 1,
                     double threshold = std::numeric_limits<double>::max(),
                     double ratio = 1.0,
                     int format = 0,
                     int modulus = 2) {
  // Read input data based on format
  if (format == 0) {
    // Input is a point cloud
    auto distance_matrix = read_point_cloud_from_R(input_data);
    
    // Initialize Ripser with parameters
    ripser::ripser<ripser::euclidean_distance_matrix> ripser_instance(
        std::move(distance_matrix), dim_max, threshold, ratio, modulus);
    
    // Compute persistence barcodes
    auto barcodes = ripser_instance.compute_barcodes();
    
    // Return barcodes as DataFrame
    return convert_barcodes_to_R(barcodes);
  } else if (format == 1) {
    // Input is a distance matrix
    auto distance_matrix = read_distance_matrix_from_R(input_data);
    
    // Initialize Ripser with parameters
    ripser::ripser<ripser::compressed_lower_distance_matrix> ripser_instance(
        std::move(distance_matrix), dim_max, threshold, ratio, modulus);
    
    // Compute persistence barcodes
    auto barcodes = ripser_instance.compute_barcodes();
    
    // Return barcodes as DataFrame
    return convert_barcodes_to_R(barcodes);
  } else {
    Rcpp::stop("Invalid format parameter");
  }
}
