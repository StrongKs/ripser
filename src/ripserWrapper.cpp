<<<<<<< HEAD
// Include necessary libraries
#include <Rcpp.h> // Needed for Rcpp bindings to interact with R
#include <fstream> // For handling file input (to read distance matrices)
#include <string>  // For working with strings
#include <vector>  // To store collections of data, like distance values
#include <iostream> // For console output if we need to debug

using namespace Rcpp;

// [[Rcpp::export]]
List computePersistenceBarcodesR(const std::string& filename, int dim_max = 1,
                                 double threshold = std::numeric_limits<double>::max(),
                                 float ratio = 1.0) {
  // This function replaces the `main` function in Ripser so that R users can call it directly
  
  // Step 1: Open the input file
  std::ifstream file_stream(filename);
  if (file_stream.fail()) {
    // Throw an error if the file can't be opened
    Rcpp::stop("Couldn't open file " + filename);
  }
  
  // Step 2: Read data from the file
  // The original Ripser code supports different formats, so we need to parse accordingly.
  // For now, just assume we're dealing with a simple distance matrix.
  // TODO: Update this part to properly handle all the supported formats.
  
  // Placeholder: Reading values into a vector (update this with proper parsing logic)
  std::vector<double> distances;
  double value;
  while (file_stream >> value) {
    distances.push_back(value);
    file_stream.ignore(); // Ignore commas or other delimiters
  }
  
  // Step 3: Process the distance matrix using Ripser's logic
  // This is where we'll call the actual persistence computation once the Ripser code is refactored.
  
  // Placeholder: Just adding a dummy persistence pair for now
  std::vector<std::pair<int, int>> persistence_pairs; // Replace with real data
  persistence_pairs.push_back({0, 1}); // Example pair
  
  // Step 4: Gather the results into a format that R can understand and return
  // The results will be a list of pairs representing (birth, death) of features
  List persistenceResults = List::create(Named("dimension") = dim_max,
                                         Named("pairs") = persistence_pairs);
  
  return persistenceResults;
}

/*
 Explanation of Key Steps:
 
 1. Include Necessary Libraries:
 - We include `Rcpp.h` to interact between C++ and R.
 - Other standard C++ libraries are included for file handling, strings, and storing collections of data.
 
 2. Function Definition with Rcpp Export:
 - `computePersistenceBarcodesR` is exported using `[[Rcpp::export]]` so it can be called from R.
 - It takes a filename, `dim_max`, `threshold`, and `ratio` as inputs—similar to the original Ripser command line tool.
 
 3. File Handling:
 - We use `std::ifstream` to open the input file.
 - If the file can't be opened, `Rcpp::stop` will throw an error in R to inform the user.
 
 4. Data Reading:
 - We read distance values from the file.
 - This part is just a placeholder right now—Ripser can handle multiple formats, so this logic will need to be expanded.
 
 5. Persistence Computation:
 - This step is just a placeholder. We'll need to bring in the actual Ripser code to compute persistence pairs.
 
 6. Returning the Results:
 - We return an `Rcpp::List`, which is an easy way to pass structured data back to R.
 - Using `Named` labels helps make the output more readable for R users.
 */
=======
// [[Rcpp::plugins(cpp11)]]

#include <Rcpp.h>
#include <vector>
#include <string>
#include <limits>
#include <cmath>
#include <algorithm>
#include <cassert>

using namespace Rcpp;

// Basic types and constants
typedef double value_t_ripser;
typedef int64_t index_t_ripser;
typedef uint8_t coefficient_t_ripser;
static const size_t num_coefficient_bits = 8;

// Quick prime check
bool is_prime(const coefficient_t_ripser n) {
  if (!(n & 1) || n < 2) return n == 2;
  for (coefficient_t_ripser p = 3, q = n / p, r = n % p; p <= q; p += 2, q = n / p, r = n % p)
    if (!r) return false;
    return true;
}

// Binomial coefficient table
class binomial_coeff_table {
  std::vector<std::vector<index_t_ripser>> B;
  index_t_ripser n_max, k_max;
public:
  binomial_coeff_table(index_t_ripser n, index_t_ripser k) : n_max(n), k_max(k) {
    B.resize(n + 1);
    for (index_t_ripser i = 0; i <= n; i++) {
      B[i].resize(k + 1);
      for (index_t_ripser j = 0; j <= std::min(i, k); j++) {
        if (j == 0 || j == i) B[i][j] = 1;
        else B[i][j] = B[i - 1][j - 1] + B[i - 1][j];
      }
    }
  }
  index_t_ripser operator()(index_t_ripser n, index_t_ripser k) const {
    assert(n <= n_max);
    assert(k <= k_max);
    return B[n][k];
  }
};

// Multiplicative inverses mod p
std::vector<coefficient_t_ripser> multiplicative_inverse_vector(const coefficient_t_ripser m) {
  std::vector<coefficient_t_ripser> inv(m);
  inv[1] = 1;
  for (coefficient_t_ripser a = 2; a < m; ++a) {
    inv[a] = m - (inv[m % a] * (m / a)) % m;
  }
  return inv;
}

// Used in simplex enumeration
index_t_ripser get_next_vertex(index_t_ripser& v, index_t_ripser idx, index_t_ripser k,
                               const binomial_coeff_table& binom) {
  if (binom(v, k) > idx) {
    index_t_ripser count = v;
    while (count > 0) {
      index_t_ripser i = v;
      index_t_ripser step = count >> 1;
      i -= step;
      if (binom(i, k) > idx) {
        v = --i;
        count -= step + 1;
      } else {
        count = step;
      }
    }
  }
  assert(binom(v, k) <= idx);
  assert(binom(v + 1, k) > idx);
  return v;
}

// Extracting the vertices of a simplex
template <typename OutputIterator>
OutputIterator get_simplex_vertices(index_t_ripser idx, index_t_ripser dim, index_t_ripser v,
                                    const binomial_coeff_table& binom, OutputIterator out) {
  --v;
  for (index_t_ripser k = dim + 1; k > 0; --k) {
    get_next_vertex(v, idx, k, binom);
    *out++ = v;
    idx -= binom(v, k);
  }
  return out;
}

std::vector<index_t_ripser> vertices_of_simplex(index_t_ripser simplex_idx, index_t_ripser dim,
                                                index_t_ripser n, const binomial_coeff_table& binom) {
  std::vector<index_t_ripser> verts;
  get_simplex_vertices(simplex_idx, dim, n, binom, std::back_inserter(verts));
  return verts;
}

// Entry structure
#pragma pack(1)
struct entry_t {
  index_t_ripser index : 8 * sizeof(index_t_ripser) - num_coefficient_bits;
  coefficient_t_ripser coefficient : num_coefficient_bits;
  entry_t(index_t_ripser i, coefficient_t_ripser c) : index(i), coefficient(c) {}
  entry_t(index_t_ripser i) : index(i), coefficient(0) {}
  entry_t() : index(0), coefficient(0) {}
};
#pragma pack()

inline entry_t make_entry(index_t_ripser i, coefficient_t_ripser c) { return entry_t(i, c); }
inline index_t_ripser get_index(const entry_t& e) { return e.index; }
inline index_t_ripser get_coefficient(const entry_t& e) { return e.coefficient; }
inline void set_coefficient(entry_t& e, coefficient_t_ripser c) { e.coefficient = c; }

const entry_t& get_entry(const entry_t& e) { return e; }

// Compares indices
template <typename Entry> 
struct smaller_index {
  bool operator()(const Entry& a, const Entry& b) { return get_index(a) < get_index(b); }
};

// Diameters
class diameter_index_t : public std::pair<value_t_ripser, index_t_ripser> {
public:
  diameter_index_t() : std::pair<value_t_ripser, index_t_ripser>() {}
  diameter_index_t(std::pair<value_t_ripser, index_t_ripser> p) : std::pair<value_t_ripser, index_t_ripser>(p) {}
};
inline value_t_ripser get_diameter(const diameter_index_t& i) { return i.first; }
inline index_t_ripser get_index(const diameter_index_t& i) { return i.second; }

class diameter_entry_t : public std::pair<value_t_ripser, entry_t> {
public:
  diameter_entry_t(std::pair<value_t_ripser, entry_t> p) : std::pair<value_t_ripser, entry_t>(p) {}
  diameter_entry_t(entry_t e) : std::pair<value_t_ripser, entry_t>(0, e) {}
  diameter_entry_t() : diameter_entry_t(0) {}
  diameter_entry_t(value_t_ripser d, index_t_ripser i, coefficient_t_ripser c)
    : std::pair<value_t_ripser, entry_t>(d, make_entry(i, c)) {}
  diameter_entry_t(const diameter_index_t& di, coefficient_t_ripser c)
    : std::pair<value_t_ripser, entry_t>(get_diameter(di), make_entry(get_index(di), c)) {}
  diameter_entry_t(const diameter_index_t& di) : diameter_entry_t(di, 1) {}
};

inline entry_t& get_entry(const diameter_entry_t& p) { return p.second; }
entry_t& get_entry(diameter_entry_t& p) { return p.second; }
inline value_t_ripser get_diameter(const diameter_entry_t& p) { return p.first; }
inline index_t_ripser get_index(const diameter_entry_t& p)    { return p.second.index; }
inline coefficient_t_ripser get_coefficient(const diameter_entry_t& p) { return p.second.coefficient; }
inline void set_coefficient(diameter_entry_t& p, coefficient_t_ripser c) { p.second.coefficient = c; }

// Sort by diameter desc, then index asc
template <typename E>
struct greater_diameter_or_smaller_index {
  bool operator()(const E& a, const E& b) {
    return (get_diameter(a) > get_diameter(b)) ||
      ((get_diameter(a) == get_diameter(b)) && (get_index(a) < get_index(b)));
  }
};

// Union-Find for connected components
class union_find {
  std::vector<index_t_ripser> parent;
  std::vector<uint8_t> rank;
public:
  union_find(index_t_ripser n) : parent(n), rank(n, 0) {
    for (index_t_ripser i = 0; i < n; ++i) {
      parent[i] = i;
    }
  }
  index_t_ripser find(index_t_ripser x) {
    if (parent[x] != x) parent[x] = find(parent[x]);
    return parent[x];
  }
  void link(index_t_ripser x, index_t_ripser y) {
    index_t_ripser rx = find(x), ry = find(y);
    if (rx == ry) return;
    if (rank[rx] < rank[ry]) parent[rx] = ry;
    else {
      parent[ry] = rx;
      if (rank[rx] == rank[ry]) rank[rx]++;
    }
  }
};

// Compressed distance matrices
enum compressed_matrix_layout { LOWER_TRIANGULAR, UPPER_TRIANGULAR };

template <compressed_matrix_layout Layout>
class compressed_distance_matrix {
public:
  std::vector<value_t_ripser> distances;
  std::vector<value_t_ripser*> rows;
  
  compressed_distance_matrix() {}
  compressed_distance_matrix(std::vector<value_t_ripser>&& dist_in)
    : distances(std::move(dist_in)),
      rows((1 + std::sqrt(1 + 8 * distances.size())) / 2) {
    assert(distances.size() == size() * (size() - 1) / 2);
    init_rows();
  }
  size_t size() const { return rows.size(); }
  void init_rows();
  value_t_ripser operator()(index_t_ripser i, index_t_ripser j) const;
};

template <>
void compressed_distance_matrix<LOWER_TRIANGULAR>::init_rows() {
  value_t_ripser* ptr = distances.data();
  for (index_t_ripser i = 1; i < size(); ++i) {
    rows[i] = ptr;
    ptr += i;
  }
}

template <>
void compressed_distance_matrix<UPPER_TRIANGULAR>::init_rows() {
  value_t_ripser* ptr = distances.data() - 1;
  for (index_t_ripser i = 0; i < size() - 1; ++i) {
    rows[i] = ptr;
    ptr += (size() - i - 2);
  }
}

template <>
value_t_ripser compressed_distance_matrix<LOWER_TRIANGULAR>::operator()(index_t_ripser i, index_t_ripser j) const {
  if (i > j) std::swap(i, j);
  if (i == j) return 0.0;
  return rows[j][i];
}

template <>
value_t_ripser compressed_distance_matrix<UPPER_TRIANGULAR>::operator()(index_t_ripser i, index_t_ripser j) const {
  if (i > j) std::swap(i, j);
  if (i == j) return 0.0;
  return rows[i][j];
}

typedef compressed_distance_matrix<LOWER_TRIANGULAR> compressed_lower_distance_matrix;
typedef compressed_distance_matrix<UPPER_TRIANGULAR> compressed_upper_distance_matrix;

// From point cloud
compressed_lower_distance_matrix getPointCloud(const NumericMatrix& mat) {
  int n = mat.nrow(), d = mat.ncol();
  std::vector<value_t_ripser> lower_tri;
  lower_tri.reserve(n*(n+1)/2);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j <= i; ++j) {
      double dist_sq = 0.0;
      for (int k = 0; k < d; ++k) {
        double diff = mat(i, k) - mat(j, k);
        dist_sq += diff*diff;
      }
      lower_tri.push_back(std::sqrt(dist_sq));
    }
  }
  return compressed_lower_distance_matrix(std::move(lower_tri));
}

// From user-supplied distance matrix
compressed_lower_distance_matrix getLowerDistMatrix(const NumericMatrix& mat) {
  int n = mat.nrow();
  std::vector<value_t_ripser> lower_tri;
  lower_tri.reserve(n*(n+1)/2);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j <= i; ++j) {
      lower_tri.push_back(mat(i, j));
    }
  }
  return compressed_lower_distance_matrix(std::move(lower_tri));
}

compressed_lower_distance_matrix read_file(const NumericMatrix& input_data, int format) {
  if (format == 0) return getPointCloud(input_data);
  else if (format == 1) return getLowerDistMatrix(input_data);
  else Rcpp::stop("Invalid format parameter (0: point cloud, 1: distance matrix)");
}

// Computes barcodes
template <typename DistanceMatrix>
NumericVector ripser_compute(const DistanceMatrix& dist, int dim, float thresh, int p) {
  if (p <= 0 || !is_prime(p)) {
    Rcpp::stop("Parameter 'modulus' must be a positive prime.");
  }
  coefficient_t_ripser mod = (coefficient_t_ripser) p;
  index_t_ripser n = dist.size();
  index_t_ripser dim_max = std::min<index_t_ripser>(dim, n-2);
  binomial_coeff_table binom(n, dim_max + 2);
  value_t_ripser threshold = (thresh > 0) ? (value_t_ripser) thresh
  : std::numeric_limits<value_t_ripser>::max();
  
  std::vector<std::vector<value_t_ripser>> intervals;
  union_find dset(n);
  
  struct comp1_t {
    const DistanceMatrix& dist;
    binomial_coeff_table& binom;
    comp1_t(const DistanceMatrix& d, binomial_coeff_table& b) : dist(d), binom(b) {}
    value_t_ripser diameter(index_t_ripser idx) const {
      std::vector<index_t_ripser> verts;
      verts.reserve(2);
      get_simplex_vertices(idx, 1, dist.size(), binom, std::back_inserter(verts));
      return dist(verts[0], verts[1]);
    }
  } comp1(dist, binom);
  
  std::vector<diameter_index_t> edges;
  edges.reserve(binom(n,2));
  for (index_t_ripser idx = binom(n, 2); idx-- > 0; ) {
    value_t_ripser diam = comp1.diameter(idx);
    if (diam <= threshold) edges.push_back({ diam, idx });
  }
  std::sort(edges.rbegin(), edges.rend(), greater_diameter_or_smaller_index<diameter_index_t>());
  
  for (auto & e : edges) {
    double d = get_diameter(e);
    std::vector<index_t_ripser> verts;
    verts.reserve(2);
    get_simplex_vertices(get_index(e), 1, n, binom, std::back_inserter(verts));
    index_t_ripser r1 = dset.find(verts[0]), r2 = dset.find(verts[1]);
    if (r1 != r2) {
      if (d > 0.0) {
        intervals.push_back({0, 0.0, d});
      }
      dset.link(r1, r2);
    }
  }
  
  NumericVector out(intervals.size() * 3);
  int pos = 0;
  for (auto &iv : intervals) {
    out[pos++] = iv[0];
    out[pos++] = iv[1];
    out[pos++] = iv[2];
  }
  return out;
}

// [[Rcpp::export]]
DataFrame ripser_cpp(const NumericMatrix& input_data,
                     int dim_max = 1,
                     double threshold = std::numeric_limits<double>::max(),
                     double ratio = 1.0,
                     int format = 0,
                     int modulus = 2) {
  compressed_lower_distance_matrix dist = read_file(input_data, format);
  float thresh_f = static_cast<float>(threshold);
  NumericVector intervals = ripser_compute(dist, dim_max, thresh_f, modulus);
  int n = intervals.size() / 3;
  IntegerVector dim_col(n);
  NumericVector birth_col(n), death_col(n);
  
  for (int i = 0; i < n; i++) {
    dim_col[i]   = (int) intervals[3*i];
    birth_col[i] = intervals[3*i + 1];
    death_col[i] = intervals[3*i + 2];
  }
  return DataFrame::create(
    Named("dimension") = dim_col,
    Named("birth")     = birth_col,
    Named("death")     = death_col
  );
}
>>>>>>> origin/Second-Phase
