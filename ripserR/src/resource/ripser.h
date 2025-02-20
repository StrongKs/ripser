//
// Created by Kent Phipps on 2/16/25.
//

#ifndef RIPSERR_RIPSER_H
#define RIPSERR_RIPSER_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <unordered_map>

#include "types.h"
#include "sparse_distance_matrix.h"
#include "compressed_distance_matrix.h"
#include "union_find.h"
#include "binomial_coeff_table.h"
#include "compressed_sparse_matrix.h"

template <class Key, class T, class H, class E> using hash_map = std::unordered_map<Key, T, H, E>;
template <class Key> using hash_templated = std::hash<Key>;

struct entry_hash {
    std::size_t operator()(const entry_t& e) const { return hash_templated<index_t>()(::get_index(e)); }
};

struct equal_index {
    bool operator()(const entry_t& e, const entry_t& f) const {
        return ::get_index(e) == ::get_index(f);
    }
};

// Ripser template class declaration
template <typename DistanceMatrix>
class ripser {
private:
    const DistanceMatrix dist;
    const index_t n, dim_max;
    const value_t threshold;
    const float ratio;
    const coefficient_t modulus;
    const binomial_coeff_table binomial_coeff;
    const std::vector<coefficient_t> multiplicative_inverse;
    mutable std::vector<diameter_entry_t> cofacet_entries;
    mutable std::vector<index_t> vertices;
    typedef hash_map<entry_t, size_t, entry_hash, equal_index> entry_hash_map;



public:
    ripser(DistanceMatrix&& _dist, index_t _dim_max, value_t _threshold, float _ratio, coefficient_t _modulus);
    void compute_barcodes();
    std::vector<diameter_index_t> get_edges();

    class simplex_boundary_enumerator;  // Forward declaration
    class simplex_coboundary_enumerator;

    void compute_dim_0_pairs(std::vector<diameter_index_t> &edges, std::vector<diameter_index_t> &columns_to_reduce);
//    template <typename OutputIterator>
//    static OutputIterator get_simplex_vertices(index_t idx, const index_t dim, index_t n, OutputIterator out) ;
    index_t get_max_vertex(const index_t idx, const index_t k, const index_t n) const;
    diameter_entry_t get_zero_apparent_cofacet(const diameter_entry_t simplex, const index_t dim);
    diameter_entry_t get_zero_pivot_cofacet(const diameter_entry_t simplex, const index_t dim);
    diameter_entry_t get_zero_pivot_facet(const diameter_entry_t simplex, const index_t dim);
    void compute_pairs(const std::vector<diameter_index_t>& columns_to_reduce,
                       entry_hash_map& pivot_column_index, const index_t dim);
    template <typename Column>
    diameter_entry_t init_coboundary_and_get_pivot(const diameter_entry_t simplex,
                                                   Column& working_coboundary, const index_t& dim,
                                                   entry_hash_map& pivot_column_index);
    diameter_entry_t get_zero_apparent_facet(const diameter_entry_t simplex, const index_t dim);
    template <typename Column>
    void add_coboundary(compressed_sparse_matrix<diameter_entry_t>& reduction_matrix,
                        const std::vector<diameter_index_t>& columns_to_reduce,
                        const size_t index_column_to_add, const coefficient_t factor,
                        const size_t& dim, Column& working_reduction_column,
                        Column& working_coboundary);
    template <typename Column>
    diameter_entry_t get_pivot(Column& column);
    template <typename Column>
    void add_simplex_coboundary(const diameter_entry_t simplex, const index_t& dim,
                                Column& working_reduction_column, Column& working_coboundary);
    template <typename Column>
    diameter_entry_t pop_pivot(Column& column);
    void assemble_columns_to_reduce(std::vector<diameter_index_t>& simplices,
                                    std::vector<diameter_index_t>& columns_to_reduce,
                                    entry_hash_map& pivot_column_index, index_t dim);
    index_t get_edge_index(const index_t i, const index_t j) const;
    bool is_in_zero_apparent_pair(const diameter_entry_t simplex, const index_t dim);
};

template <typename OutputIterator>
OutputIterator get_simplex_vertices(index_t idx, const index_t dim, index_t n, OutputIterator out);

// Explicit template specializations for compressed_lower_distance_matrix and sparse_distance_matrix
//template <>
//class ripser<compressed_lower_distance_matrix>;
//
//template <>
//class ripser<sparse_distance_matrix>;

// Include template implementation
#include "ripser.tpp"

#endif // RIPSERR_RIPSER_H
