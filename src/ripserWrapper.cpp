// [[Rcpp::plugins(cpp11)]]

#include <Rcpp.h>
#include "ripser_short.h" // Provides ripser_compute, getPointCloud, getLowerDistMatrix
#include <vector>
#include <string>

using namespace Rcpp;

// [[Rcpp::export]]
DataFrame ripser_cpp(const NumericMatrix& input_data,
                     int dim_max = 1,
                     double threshold = std::numeric_limits<double>::max(),
                     double ratio = 1.0,
                     int format = 0,
                     int modulus = 2) {
  // Determine which input format we're using
  compressed_lower_distance_matrix dist;
  if (format == 0) {
    // Input is a point cloud
    dist = getPointCloud(input_data);
  } else if (format == 1) {
    // Input is a distance matrix
    dist = getLowerDistMatrix(input_data);
  } else {
    Rcpp::stop("Invalid format parameter (0: point cloud, 1: distance matrix)");
  }
  
  // Convert threshold to float for ripser_compute
  float thresh_f = static_cast<float>(threshold);
  
  // Compute persistence intervals using ripser_compute
  // Returns a NumericVector with (dimension, birth, death) triples
  NumericVector intervals = ripser_compute(dist, dim_max, thresh_f, modulus);
  
  // intervals contains dim, birth, death in sequence
  int n = intervals.size() / 3;
  IntegerVector dim_col(n);
  NumericVector birth_col(n);
  NumericVector death_col(n);
  
  for (int i = 0; i < n; i++) {
    dim_col[i] = static_cast<int>(intervals[3*i]);
    birth_col[i] = intervals[3*i + 1];
    death_col[i] = intervals[3*i + 2];
  }
  
  // Return the barcodes as a DataFrame
  return DataFrame::create(
    Named("dimension") = dim_col,
    Named("birth") = birth_col,
    Named("death") = death_col
  );
}
