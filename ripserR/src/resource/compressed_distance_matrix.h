//
// Created by Kent Phipps on 2/16/25.
//

#ifndef COMPRESSED_DISTANCE_MATRIX_H
#define COMPRESSED_DISTANCE_MATRIX_H

#include <vector>
#include <cmath>
#include <cassert>
#include "types.h"  // Assuming index_t and value_t are defined there

enum compressed_matrix_layout { LOWER_TRIANGULAR, UPPER_TRIANGULAR };

template <compressed_matrix_layout Layout>
struct compressed_distance_matrix {
    std::vector<value_t> distances;
    std::vector<value_t*> rows;

    compressed_distance_matrix(std::vector<value_t>&& _distances);

    template <typename DistanceMatrix>
    compressed_distance_matrix(const DistanceMatrix& mat);

    value_t operator()(const index_t i, const index_t j) const;
    size_t size() const;
    void init_rows();
};

// Typedefs for ease of use
typedef compressed_distance_matrix<LOWER_TRIANGULAR> compressed_lower_distance_matrix;
typedef compressed_distance_matrix<UPPER_TRIANGULAR> compressed_upper_distance_matrix;

// Include template implementations
#include "compressed_distance_matrix.tpp"

#endif // COMPRESSED_DISTANCE_MATRIX_H