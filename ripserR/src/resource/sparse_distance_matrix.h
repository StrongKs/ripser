//
// Created by Kent Phipps on 2/16/25.
//

#ifndef SPARSE_DISTANCE_MATRIX_H
#define SPARSE_DISTANCE_MATRIX_H

#include <vector>
#include <limits>
#include <algorithm>
#include "types.h"  // Assuming index_diameter_t, index_t, value_t are defined here.

struct sparse_distance_matrix {
    std::vector<std::vector<index_diameter_t>> neighbors;
    index_t num_edges;

    // Constructor declarations
    sparse_distance_matrix(std::vector<std::vector<index_diameter_t>>&& _neighbors, index_t _num_edges);

    template <typename DistanceMatrix>
    sparse_distance_matrix(const DistanceMatrix& mat, const value_t threshold);

    // Function declarations
    value_t operator()(const index_t i, const index_t j) const;
    size_t size() const;
};

// Include the template implementation
#include "sparse_distance_matrix.tpp"

#endif // SPARSE_DISTANCE_MATRIX_H
