// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include "ripser_short.h" // has getPointCloud() / getLowerDistMatrix(), etc.
#include <vector>
#include <string>

using namespace Rcpp;

// [[Rcpp::export]]
DataFrame ripser_cpp(const NumericMatrix& input_data,
                     int dim_max = 1,
                     double threshold = 1.7976931348623157e+308,
                     double ratio = 1.0,
                     int format = 0,
                     int modulus = 2)
{
	// 1) Construct the distance matrix (no empty constructor!)
	compressed_lower_distance_matrix dist =
	    (format == 0) ? getPointCloud(input_data)
	                  : getLowerDistMatrix(input_data);

	// 2) Convert threshold to float
	float thresh_f = static_cast<float>(threshold);

	// 3) Compute persistence intervals
	NumericVector intervals = ripser_compute(dist, dim_max, thresh_f, modulus);

	// 4) Convert intervals => (dimension, birth, death)
	int n = intervals.size() / 3;
	IntegerVector dim_col(n);
	NumericVector birth_col(n), death_col(n);

	for (int i = 0; i < n; i++) {
		dim_col[i]   = (int)intervals[3*i];
		birth_col[i] = intervals[3*i + 1];
		death_col[i] = intervals[3*i + 2];
	}

	// 5) Return as a DataFrame
	return DataFrame::create(
	    Named("dimension") = dim_col,
	    Named("birth")     = birth_col,
	    Named("death")     = death_col
	);
}
