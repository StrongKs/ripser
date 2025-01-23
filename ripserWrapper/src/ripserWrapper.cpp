
#include <Rcpp.h>
#include "ripser.cpp" // Include ripser.cpp functions here
#include <vector>
#include <string>

using namespace Rcpp;

// [[Rcpp::plugins(cpp11)]]
List computePersistenceBarcodesR(NumericMatrix coordinates, int dim_max = 1,
                                 double threshold = std::numeric_limits<double>::max(),
                                 float ratio = 1.0) {
  // Step 1: Convert R's NumericMatrix to a C++ vector of vectors
  std::vector<std::vector<double>> points(coordinates.nrow(), std::vector<double>(coordinates.ncol()));
  for (int i = 0; i < coordinates.nrow(); i++) {
    for (int j = 0; j < coordinates.ncol(); j++) {
      points[i][j] = coordinates(i, j);
    }
  }

  // Step 2: Instantiate the appropriate Ripser class with parameters
  // Adjust based on whether you're dealing with a distance matrix or point cloud
  ripser::euclidean_distance_matrix distance_matrix(std::move(points));
  ripser<ripser::euclidean_distance_matrix> ripser_instance(
      std::move(distance_matrix), dim_max, threshold, ratio, 2);

  // Step 3: Compute barcodes
  ripser_instance.compute_barcodes();

  // Step 4: Extract the persistence pairs and format them for R
  std::vector<std::pair<double, double>> persistence_pairs = ripser_instance.get_persistence_pairs();

  // Convert persistence pairs to R-compatible structures
  std::vector<double> birth_times;
  std::vector<double> death_times;
  for (const auto& pair : persistence_pairs) {
    birth_times.push_back(pair.first);
    death_times.push_back(pair.second);
  }

  //  this is to create an Rcpp DataFrame to hold the persistence pairs

  DataFrame result = DataFrame::create(
    Named("birth") = birth_times,
    Named("death") = death_times,
    Named("dimension") = dim_max
  );

  return result; // Return the DataFrame to R
}
