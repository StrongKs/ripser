#ifndef RIPSERR_TPP
#define RIPSERR_TPP

#include "ripser.h"

// Implementation of template function get_max
template <class Predicate>
index_t get_max(index_t top, const index_t bottom, const Predicate pred) {
    if (!pred(top)) {
        index_t count = top - bottom;
        while (count > 0) {
            index_t step = count >> 1, mid = top - step;
            if (!pred(mid)) {
                top = mid - 1;
                count -= step + 1;
            } else {
                count = step;
            }
        }
    }
    return top;
}

// Helper function
std::vector<coefficient_t> multiplicative_inverse_vector(const coefficient_t m) {
    std::vector<coefficient_t> inverse(m);
    inverse[1] = 1;
    // m = a * (m / a) + m % a
    // Multipying with inverse(a) * inverse(m % a):
    // 0 = inverse(m % a) * (m / a) + inverse(a)  (mod m)
    for (coefficient_t a = 2; a < m; ++a) inverse[a] = m - (inverse[m % a] * (m / a)) % m;
    return inverse;
}

// Template Constructor
template <typename DistanceMatrix>
ripser<DistanceMatrix>::ripser(DistanceMatrix&& _dist, index_t _dim_max, value_t _threshold, float _ratio, coefficient_t _modulus)
    : dist(std::move(_dist)), n(dist.size()),
      dim_max(std::min(_dim_max, index_t(dist.size() - 2))), threshold(_threshold),
      ratio(_ratio), modulus(_modulus), binomial_coeff(n, dim_max + 2),
      multiplicative_inverse(multiplicative_inverse_vector(_modulus)) {}

// Implementation of `get_max_vertex`
template <typename DistanceMatrix>
index_t ripser<DistanceMatrix>::get_max_vertex(const index_t idx, const index_t k, const index_t n) const {
    return get_max(n, k - 1, [&](index_t w) -> bool { return (binomial_coeff(w, k) <= idx); });
}

// Implementation of `get_simplex_vertices`
template <typename DistanceMatrix>
template <typename OutputIterator>
OutputIterator ripser<DistanceMatrix>::get_simplex_vertices(index_t idx, const index_t dim, index_t n, OutputIterator out) const {
    --n;
    for (index_t k = dim + 1; k > 1; --k) {
        n = get_max_vertex(idx, k, n);
        *out++ = n;
        idx -= binomial_coeff(n, k);
    }
    *out = idx;
    return out;
}

// Definition of `simplex_boundary_enumerator`
template <typename DistanceMatrix>
class ripser<DistanceMatrix>::simplex_boundary_enumerator {
private:
    index_t idx_below, idx_above, j, k;
    diameter_entry_t simplex;
    index_t dim;
    const coefficient_t modulus;
    const binomial_coeff_table& binomial_coeff;
    const ripser& parent;

public:
    simplex_boundary_enumerator(const diameter_entry_t _simplex, const index_t _dim, const ripser& _parent)
            : idx_below(get_index(_simplex)), idx_above(0), j(_parent.n - 1), k(_dim),
              simplex(_simplex), modulus(_parent.modulus), binomial_coeff(_parent.binomial_coeff), parent(_parent) {}

    simplex_boundary_enumerator(const index_t _dim, const ripser& _parent)
            : simplex_boundary_enumerator(-1, _dim, _parent) {}

    void set_simplex(const diameter_entry_t _simplex, const index_t _dim) {
        idx_below = get_index(_simplex);
        idx_above = 0;
        j = parent.n - 1;
        k = _dim;
        simplex = _simplex;
        dim = _dim;
    }

    bool has_next() { return (k >= 0); }

    diameter_entry_t next() {
        j = parent.get_max_vertex(idx_below, k + 1, j);
        index_t face_index = idx_above - binomial_coeff(j, k + 1) + idx_below;
        value_t face_diameter = parent.compute_diameter(face_index, dim - 1);

        coefficient_t face_coefficient = (k & 1 ? -1 + modulus : 1) * get_coefficient(simplex) % modulus;

        idx_below -= binomial_coeff(j, k + 1);
        idx_above += binomial_coeff(j, k);
        --k;

        return diameter_entry_t(face_diameter, face_index, face_coefficient);
    }
};

// Implementation of `get_zero_pivot_cofacet`
template <typename DistanceMatrix>
diameter_entry_t ripser<DistanceMatrix>::get_zero_pivot_cofacet(const diameter_entry_t simplex, const index_t dim) {
    static simplex_coboundary_enumerator cofacets(*this);
    cofacets.set_simplex(simplex, dim);
    while (cofacets.has_next()) {
        diameter_entry_t cofacet = cofacets.next();
        if (get_diameter(cofacet) == get_diameter(simplex)) return cofacet;
    }
    return diameter_entry_t(-1);
}

// Implementation of `get_zero_pivot_facet`
template <typename DistanceMatrix>
diameter_entry_t ripser<DistanceMatrix>::get_zero_pivot_facet(const diameter_entry_t simplex, const index_t dim) {
    static simplex_boundary_enumerator facets(0, *this);
    facets.set_simplex(simplex, dim);
    while (facets.has_next()) {
        diameter_entry_t facet = facets.next();
        if (get_diameter(facet) == get_diameter(simplex)) return facet;
    }
    return diameter_entry_t(-1);
}

// Implementation of `get_zero_apparent_cofacet`
template <typename DistanceMatrix>
diameter_entry_t ripser<DistanceMatrix>::get_zero_apparent_cofacet(const diameter_entry_t simplex, const index_t dim) {
    diameter_entry_t cofacet = get_zero_pivot_cofacet(simplex, dim);
    return ((get_index(cofacet) != -1) &&
            (get_index(get_zero_pivot_facet(cofacet, dim + 1)) == get_index(simplex)))
           ? cofacet
           : diameter_entry_t(-1);
}

template <typename DistanceMatrix>
void ripser<DistanceMatrix>::compute_dim_0_pairs(std::vector<diameter_index_t>& edges,
                         std::vector<diameter_index_t>& columns_to_reduce) {
#ifdef PRINT_PERSISTENCE_PAIRS
    std::cout << "persistence intervals in dim 0:" << std::endl;
#endif

    union_find dset(n);

    edges = get_edges();
    std::sort(edges.rbegin(), edges.rend(),
              greater_diameter_or_smaller_index<diameter_index_t>);
    std::vector<index_t> vertices_of_edge(2);
    for (auto e : edges) {
        get_simplex_vertices(get_index(e), 1, n, vertices_of_edge.rbegin());
        index_t u = dset.find(vertices_of_edge[0]), v = dset.find(vertices_of_edge[1]);

        if (u != v) {
#ifdef PRINT_PERSISTENCE_PAIRS
            if (get_diameter(e) != 0)
					std::cout << " [0," << get_diameter(e) << ")" << std::endl;
#endif
            dset.link(u, v);
        } else if ((dim_max > 0) && (get_index(get_zero_apparent_cofacet(e, 1)) == -1))
            columns_to_reduce.push_back(e);
    }
    if (dim_max > 0) std::reverse(columns_to_reduce.begin(), columns_to_reduce.end());

#ifdef PRINT_PERSISTENCE_PAIRS
    for (index_t i = 0; i < n; ++i)
			if (dset.find(i) == i) std::cout << " [0, )" << std::endl;
#endif
}

// Implementation of `get_zero_apparent_facet`
template <typename DistanceMatrix>
diameter_entry_t ripser<DistanceMatrix>::get_zero_apparent_facet(const diameter_entry_t simplex, const index_t dim) {
    diameter_entry_t facet = get_zero_pivot_facet(simplex, dim);
    return ((get_index(facet) != -1) &&
            (get_index(get_zero_pivot_cofacet(facet, dim - 1)) == get_index(simplex)))
           ? facet
           : diameter_entry_t(-1);
}

// Implementation of `init_coboundary_and_get_pivot`
template <typename DistanceMatrix>
template <typename Column>
diameter_entry_t ripser<DistanceMatrix>::init_coboundary_and_get_pivot(const diameter_entry_t simplex,
                                                                       Column& working_coboundary,
                                                                       const index_t& dim,
                                                                       entry_hash_map& pivot_column_index) {
    static simplex_coboundary_enumerator cofacets(*this);
    bool check_for_emergent_pair = true;
    cofacet_entries.clear();
    cofacets.set_simplex(simplex, dim);

    while (cofacets.has_next()) {
        diameter_entry_t cofacet = cofacets.next();
        if (get_diameter(cofacet) <= threshold) {
            cofacet_entries.push_back(cofacet);
            if (check_for_emergent_pair && (get_diameter(simplex) == get_diameter(cofacet))) {
                if ((pivot_column_index.find(get_entry(cofacet)) == pivot_column_index.end()) &&
                    (get_index(get_zero_apparent_facet(cofacet, dim + 1)) == -1))
                    return cofacet;
                check_for_emergent_pair = false;
            }
        }
    }
    for (auto cofacet : cofacet_entries)
        working_coboundary.push(cofacet);

    return get_pivot(working_coboundary);
}

// Implementation of `add_coboundary`
template <typename DistanceMatrix>
template <typename Column>
void ripser<DistanceMatrix>::add_coboundary(compressed_sparse_matrix<diameter_entry_t>& reduction_matrix,
                                            const std::vector<diameter_index_t>& columns_to_reduce,
                                            const size_t index_column_to_add, const coefficient_t factor,
                                            const size_t& dim, Column& working_reduction_column,
                                            Column& working_coboundary) {
    diameter_entry_t column_to_add(columns_to_reduce[index_column_to_add], factor);
    add_simplex_coboundary(column_to_add, dim, working_reduction_column, working_coboundary);


    auto range = reduction_matrix.subrange(index_column_to_add);
    for (auto it = range.first; it != range.second; ++it) {
        set_coefficient(*it, get_coefficient(*it) * factor % modulus);
        add_simplex_coboundary(*it, dim, working_reduction_column, working_coboundary);
    }

//    // had error with :
//    for (diameter_entry_t simplex : reduction_matrix.subrange(index_column_to_add)) {
//        set_coefficient(simplex, get_coefficient(simplex) * factor % modulus);
//        add_simplex_coboundary(simplex, dim, working_reduction_column, working_coboundary);
//    }
}

// Implementation of `get_pivot`
template <typename DistanceMatrix>
template <typename Column>
diameter_entry_t ripser<DistanceMatrix>::get_pivot(Column& column) {
    diameter_entry_t result = pop_pivot(column);
    if (get_index(result) != -1) column.push(result);
    return result;
}

// Implementation of `add_simplex_coboundary`
template <typename DistanceMatrix>
template <typename Column>
void ripser<DistanceMatrix>::add_simplex_coboundary(const diameter_entry_t simplex, const index_t& dim,
                                                    Column& working_reduction_column, Column& working_coboundary) {
    static simplex_coboundary_enumerator cofacets(*this);
    working_reduction_column.push(simplex);
    cofacets.set_simplex(simplex, dim);
    while (cofacets.has_next()) {
        diameter_entry_t cofacet = cofacets.next();
        if (get_diameter(cofacet) <= threshold) working_coboundary.push(cofacet);
    }
}

// Implementation of `pop_pivot`
template <typename DistanceMatrix>
template <typename Column>
diameter_entry_t ripser<DistanceMatrix>::pop_pivot(Column& column) {
    diameter_entry_t pivot(-1);
#ifdef USE_COEFFICIENTS
    while (!column.empty()) {
        if (get_coefficient(pivot) == 0)
            pivot = column.top();
        else if (get_index(column.top()) != get_index(pivot))
            return pivot;
        else
            set_coefficient(pivot, (get_coefficient(pivot) + get_coefficient(column.top())) % modulus);
        column.pop();
    }
    return (get_coefficient(pivot) == 0) ? -1 : pivot;
#else
    while (!column.empty()) {
        pivot = column.top();
        column.pop();
        if (column.empty() || get_index(column.top()) != get_index(pivot)) return pivot;
        column.pop();
    }
    return -1;
#endif
}

// Implementation of `compute_pairs`
template <typename DistanceMatrix>
void ripser<DistanceMatrix>::compute_pairs(const std::vector<diameter_index_t>& columns_to_reduce,
                                           entry_hash_map& pivot_column_index, const index_t dim) {
#ifdef PRINT_PERSISTENCE_PAIRS
    std::cout << "persistence intervals in dim " << dim << ":" << std::endl;
#endif

    compressed_sparse_matrix<diameter_entry_t> reduction_matrix;

#ifdef INDICATE_PROGRESS
    std::chrono::steady_clock::time_point next = std::chrono::steady_clock::now() + time_step;
#endif

    for (size_t index_column_to_reduce = 0; index_column_to_reduce < columns_to_reduce.size();
         ++index_column_to_reduce) {

        diameter_entry_t column_to_reduce(columns_to_reduce[index_column_to_reduce], 1);
        value_t diameter = get_diameter(column_to_reduce);

        reduction_matrix.append_column();

        std::priority_queue<diameter_entry_t, std::vector<diameter_entry_t>,
                greater_diameter_or_smaller_index_comp<diameter_entry_t>>
                working_reduction_column, working_coboundary;

        diameter_entry_t e, pivot = init_coboundary_and_get_pivot(
                column_to_reduce, working_coboundary, dim, pivot_column_index);

        while (true) {
#ifdef INDICATE_PROGRESS
            if (std::chrono::steady_clock::now() > next) {
                std::cerr << clear_line << "reducing column " << index_column_to_reduce + 1
                          << "/" << columns_to_reduce.size() << " (diameter " << diameter << ")"
                          << std::flush;
                next = std::chrono::steady_clock::now() + time_step;
            }
#endif
            if (get_index(pivot) != -1) {
                auto pair = pivot_column_index.find(get_entry(pivot));
                if (pair != pivot_column_index.end()) {
                    entry_t other_pivot = pair->first;
                    index_t index_column_to_add = pair->second;
                    coefficient_t factor =
                            modulus - get_coefficient(pivot) *
                                      multiplicative_inverse[get_coefficient(other_pivot)] % modulus;

                    add_coboundary(reduction_matrix, columns_to_reduce, index_column_to_add,
                                   factor, dim, working_reduction_column, working_coboundary);

                    pivot = get_pivot(working_coboundary);
                } else if (get_index(e = get_zero_apparent_facet(pivot, dim + 1)) != -1) {
                    set_coefficient(e, modulus - get_coefficient(e));

                    add_simplex_coboundary(e, dim, working_reduction_column, working_coboundary);

                    pivot = get_pivot(working_coboundary);
                } else {
#ifdef PRINT_PERSISTENCE_PAIRS
                    value_t death = get_diameter(pivot);
                    if (death > diameter * ratio) {
#ifdef INDICATE_PROGRESS
                        std::cerr << clear_line << std::flush;
#endif
                        std::cout << " [" << diameter << "," << death << ")" << std::endl;
                    }
#endif
                    pivot_column_index.insert({get_entry(pivot), index_column_to_reduce});

                    while (true) {
                        diameter_entry_t e = pop_pivot(working_reduction_column);
                        if (get_index(e) == -1) break;
                        assert(get_coefficient(e) > 0);
                        reduction_matrix.push_back(e);
                    }
                    break;
                }
            } else {
#ifdef PRINT_PERSISTENCE_PAIRS
                #ifdef INDICATE_PROGRESS
                std::cerr << clear_line << std::flush;
#endif
                std::cout << " [" << diameter << ", )" << std::endl;
#endif
                break;
            }
        }
    }
#ifdef INDICATE_PROGRESS
    std::cerr << clear_line << std::flush;
#endif
}

// Implementation of `assemble_columns_to_reduce`
template <typename DistanceMatrix>
void ripser<DistanceMatrix>::assemble_columns_to_reduce(std::vector<diameter_index_t>& simplices,
                                                        std::vector<diameter_index_t>& columns_to_reduce,
                                                        entry_hash_map& pivot_column_index, index_t dim) {
#ifdef INDICATE_PROGRESS
    std::cerr << clear_line << "assembling columns" << std::flush;
    std::chrono::steady_clock::time_point next = std::chrono::steady_clock::now() + time_step;
#endif

    columns_to_reduce.clear();
    std::vector<diameter_index_t> next_simplices;

    simplex_coboundary_enumerator cofacets(*this);

    for (diameter_index_t& simplex : simplices) {
        cofacets.set_simplex(diameter_entry_t(simplex, 1), dim - 1);

        while (cofacets.has_next(false)) {
#ifdef INDICATE_PROGRESS
            if (std::chrono::steady_clock::now() > next) {
                std::cerr << clear_line << "assembling " << next_simplices.size()
                          << " columns (processing " << std::distance(&simplices[0], &simplex)
                          << "/" << simplices.size() << " simplices)" << std::flush;
                next = std::chrono::steady_clock::now() + time_step;
            }
#endif
            auto cofacet = cofacets.next();
            if (get_diameter(cofacet) <= threshold) {
                if (dim < dim_max)
                    next_simplices.push_back({get_diameter(cofacet), get_index(cofacet)});
                if (!is_in_zero_apparent_pair(cofacet, dim) &&
                    (pivot_column_index.find(get_entry(cofacet)) == pivot_column_index.end()))
                    columns_to_reduce.push_back({get_diameter(cofacet), get_index(cofacet)});
            }
        }
    }

    if (dim < dim_max) simplices.swap(next_simplices);

#ifdef INDICATE_PROGRESS
    std::cerr << clear_line << "sorting " << columns_to_reduce.size() << " columns" << std::flush;
#endif

    std::sort(columns_to_reduce.begin(), columns_to_reduce.end(),
              greater_diameter_or_smaller_index<diameter_index_t>);
#ifdef INDICATE_PROGRESS
    std::cerr << clear_line << std::flush;
#endif
}

// Compute barcodes
template <typename DistanceMatrix>
void ripser<DistanceMatrix>::compute_barcodes() {
    std::vector<diameter_index_t> simplices, columns_to_reduce;
    compute_dim_0_pairs(simplices, columns_to_reduce);

    for (index_t dim = 1; dim <= dim_max; ++dim) {
        entry_hash_map pivot_column_index;
        pivot_column_index.reserve(columns_to_reduce.size());

        compute_pairs(columns_to_reduce, pivot_column_index, dim);

        if (dim < dim_max)
            assemble_columns_to_reduce(simplices, columns_to_reduce, pivot_column_index, dim + 1);
    }
}

// Template specialization for `compressed_lower_distance_matrix`
template <>
std::vector<diameter_index_t> ripser<compressed_lower_distance_matrix>::get_edges() {
    std::vector<diameter_index_t> edges;
    std::vector<index_t> vertices(2);
    for (index_t index = binomial_coeff(n, 2); index-- > 0;) {
        get_simplex_vertices(index, 1, dist.size(), vertices.rbegin());
        value_t length = dist(vertices[0], vertices[1]);
        if (length <= threshold) edges.push_back({length, index});
    }
    return edges;
}

// Template specialization for `sparse_distance_matrix`
template <>
std::vector<diameter_index_t> ripser<sparse_distance_matrix>::get_edges() {
    std::vector<diameter_index_t> edges;
    for (index_t i = 0; i < n; ++i)
        for (auto n : dist.neighbors[i]) {
            index_t j = get_index(n);
            if (i > j) edges.push_back({get_diameter(n), get_edge_index(i, j)});
        }
    return edges;
}

// Include enumerator class implementations for each specialization
#include "simplex_coboundary_enumerator.tpp"

#endif // RIPSERR_TPP