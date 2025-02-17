
#include <Rcpp.h>
using namespace Rcpp;

//#include "resource/ripser.h"
//#include "resource/sparse_distance_matrix.h"
#include "ripserSol.cpp"

// [[Rcpp::export]]
void test_ripser_print() {
  Rcpp::Rcout << "ripser.cpp print" << std::endl;
}

// [[Rcpp::export]]
List test_ripser() {
  // Define a small example distance matrix in a sparse format
  std::vector<std::vector<index_diameter_t>> distance_data = {
    {{0, 0.0}, {1, 0.5}, {2, 0.8}},
    {{0, 0.5}, {1, 0.0}, {2, 0.6}},
    {{0, 0.8}, {1, 0.6}, {2, 0.0}}
  };

  // Parameters for Ripser
  int dim_max = 2;       // Compute persistence up to dimension 2
  float threshold = 1.0; // Maximum filtration value
  int modulus = 2;       // Z/2Z coefficients

  // Construct the sparse distance matrix
  sparse_distance_matrix matrix(std::move(distance_data), distance_data.size());

  // Create Ripser object and compute persistence
  ripser r(std::move(matrix), dim_max, threshold, modulus);
  r.compute_barcodes();

  // Retrieve persistence pairs
  std::vector<std::pair<value_t, value_t>> pairs = r.get_persistence_pairs();

  // Convert result into an R list
  List result;
  for (const auto& p : pairs) {
    result.push_back(NumericVector::create(p.first, p.second));
  }

  return result;
}
