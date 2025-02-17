#ifndef COMPRESSED_DISTANCE_MATRIX_TPP
#define COMPRESSED_DISTANCE_MATRIX_TPP

#include "compressed_distance_matrix.h"

// Constructor Implementation
template <compressed_matrix_layout Layout>
compressed_distance_matrix<Layout>::compressed_distance_matrix(std::vector<value_t>&& _distances)
        : distances(std::move(_distances)), rows((1 + std::sqrt(1 + 8 * distances.size())) / 2) {
    assert(distances.size() == size() * (size() - 1) / 2);
    init_rows();
}

// Constructor for copying from another distance matrix
template <compressed_matrix_layout Layout>
template <typename DistanceMatrix>
compressed_distance_matrix<Layout>::compressed_distance_matrix(const DistanceMatrix& mat)
        : distances(mat.size() * (mat.size() - 1) / 2), rows(mat.size()) {
    init_rows();

    for (size_t i = 1; i < size(); ++i)
        for (size_t j = 0; j < i; ++j)
            rows[i][j] = mat(i, j);
}

// Returns the size of the distance matrix
template <compressed_matrix_layout Layout>
size_t compressed_distance_matrix<Layout>::size() const {
    return rows.size();
}

// Specialization: Init rows for LOWER_TRIANGULAR
template <>
void compressed_lower_distance_matrix::init_rows() {
    value_t* pointer = &distances[0];
    for (size_t i = 1; i < size(); ++i) {
        rows[i] = pointer;
        pointer += i;
    }
}

// Specialization: Init rows for UPPER_TRIANGULAR
template <>
void compressed_upper_distance_matrix::init_rows() {
    value_t* pointer = &distances[0] - 1;
    for (size_t i = 0; i < size() - 1; ++i) {
        rows[i] = pointer;
        pointer += size() - i - 2;
    }
}

// Specialization: Operator() for LOWER_TRIANGULAR
template <>
value_t compressed_lower_distance_matrix::operator()(const index_t i, const index_t j) const {
    return i == j ? 0 : i < j ? rows[j][i] : rows[i][j];
}

// Specialization: Operator() for UPPER_TRIANGULAR
template <>
value_t compressed_upper_distance_matrix::operator()(const index_t i, const index_t j) const {
    return i == j ? 0 : i > j ? rows[j][i] : rows[i][j];
}

#endif // COMPRESSED_DISTANCE_MATRIX_TPP