//
// Created by Kent Phipps on 2/16/25.
//

#include "sparse_distance_matrix.h"

// Move constructor implementation
sparse_distance_matrix::sparse_distance_matrix(std::vector<std::vector<index_diameter_t>>&& _neighbors,
                                               index_t _num_edges)
        : neighbors(std::move(_neighbors)), num_edges(_num_edges) {}

// Function implementation for `size()`
size_t sparse_distance_matrix::size() const {
    return neighbors.size();
}

// Function implementation for `operator()`
value_t sparse_distance_matrix::operator()(const index_t i, const index_t j) const {
    auto neighbor = std::lower_bound(neighbors[i].begin(), neighbors[i].end(), index_diameter_t{j, 0});
    return (neighbor != neighbors[i].end() && get_index(*neighbor) == j)
           ? get_diameter(*neighbor)
           : std::numeric_limits<value_t>::infinity();
}
