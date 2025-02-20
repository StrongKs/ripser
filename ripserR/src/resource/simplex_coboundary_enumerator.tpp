#ifndef SIMPLEX_COBOUNDARY_ENUMERATOR_TPP
#define SIMPLEX_COBOUNDARY_ENUMERATOR_TPP

#include "ripser.h"

// Specialization for `compressed_lower_distance_matrix`
template <> class ripser<compressed_lower_distance_matrix>::simplex_coboundary_enumerator {
    index_t idx_below, idx_above, j, k;
    std::vector<index_t> vertices;
    diameter_entry_t simplex;
    const coefficient_t modulus;
    const compressed_lower_distance_matrix& dist;
    const binomial_coeff_table& binomial_coeff;
    const ripser& parent;

public:
    index_t get_max_vertex(const index_t idx, const index_t k, const index_t n) const {
        return get_max(n, k - 1, [&](index_t w) -> bool { return (binomial_coeff(w, k) <= idx); });
    }

    template <typename OutputIterator>
    OutputIterator get_simplex_vertices(index_t idx, const index_t dim, index_t n,
                                        OutputIterator out) const {
        --n;
        for (index_t k = dim + 1; k > 1; --k) {
            n = get_max_vertex(idx, k, n);
            *out++ = n;
            idx -= binomial_coeff(n, k);
        }
        *out = idx;
        return out;
    }

    simplex_coboundary_enumerator(const diameter_entry_t _simplex, const index_t _dim,
                                  const ripser& _parent)
            : modulus(_parent.modulus), dist(_parent.dist),
              binomial_coeff(_parent.binomial_coeff), parent(_parent) {
        if (get_index(_simplex) != -1)
            get_simplex_vertices(get_index(_simplex), _dim, parent.n, vertices.rbegin());
    }

    simplex_coboundary_enumerator(const ripser& _parent) : modulus(_parent.modulus), dist(_parent.dist),
                                                           binomial_coeff(_parent.binomial_coeff), parent(_parent) {}

    void set_simplex(const diameter_entry_t _simplex, const index_t _dim) {
        idx_below = get_index(_simplex);
        idx_above = 0;
        j = parent.n - 1;
        k = _dim + 1;
        simplex = _simplex;
        vertices.resize(_dim + 1);
        get_simplex_vertices(get_index(_simplex), _dim, parent.n, vertices.rbegin());
    }

    bool has_next(bool all_cofacets = true) {
        return (j >= k && (all_cofacets || binomial_coeff(j, k) > idx_below));
    }

    diameter_entry_t next() {
        while ((binomial_coeff(j, k) <= idx_below)) {
            idx_below -= binomial_coeff(j, k);
            idx_above += binomial_coeff(j, k + 1);
            --j;
            --k;
            assert(k != -1);
        }
        value_t cofacet_diameter = get_diameter(simplex);
        for (index_t i : vertices) cofacet_diameter = std::max(cofacet_diameter, dist(j, i));
        index_t cofacet_index = idx_above + binomial_coeff(j--, k + 1) + idx_below;
        coefficient_t cofacet_coefficient =
                (k & 1 ? modulus - 1 : 1) * get_coefficient(simplex) % modulus;
        return diameter_entry_t(cofacet_diameter, cofacet_index, cofacet_coefficient);
    }
};

// Specialization for `sparse_distance_matrix`
template <> class ripser<sparse_distance_matrix>::simplex_coboundary_enumerator {
    index_t idx_below, idx_above, k;
    std::vector<index_t> vertices;
    diameter_entry_t simplex;
    const coefficient_t modulus;
    const sparse_distance_matrix& dist;
    const binomial_coeff_table& binomial_coeff;
    std::vector<std::vector<index_diameter_t>::const_reverse_iterator> neighbor_it;
    std::vector<std::vector<index_diameter_t>::const_reverse_iterator> neighbor_end;
    index_diameter_t neighbor;
    const ripser& parent;

public:
    simplex_coboundary_enumerator(const diameter_entry_t _simplex, const index_t _dim,
                                  const ripser& _parent)
            : modulus(_parent.modulus), dist(_parent.dist),
              binomial_coeff(_parent.binomial_coeff), parent(_parent) {
        if (get_index(_simplex) != -1) set_simplex(_simplex, _dim);
    }

    simplex_coboundary_enumerator(const ripser& _parent)
            : modulus(_parent.modulus), dist(_parent.dist),
              binomial_coeff(_parent.binomial_coeff), parent(_parent) {}

    void set_simplex(const diameter_entry_t _simplex, const index_t _dim) {
        idx_below = get_index(_simplex);
        idx_above = 0;
        k = _dim + 1;
        simplex = _simplex;
        vertices.resize(_dim + 1);
        get_simplex_vertices(idx_below, _dim, parent.n, vertices.rbegin());

        neighbor_it.resize(_dim + 1);
        neighbor_end.resize(_dim + 1);
        for (index_t i = 0; i <= _dim; ++i) {
            auto v = vertices[i];
            neighbor_it[i] = dist.neighbors[v].rbegin();
            neighbor_end[i] = dist.neighbors[v].rend();
        }
    }

    bool has_next(bool all_cofacets = true) {
        for (auto &it0 = neighbor_it[0], &end0 = neighbor_end[0]; it0 != end0; ++it0) {
            neighbor = *it0;
            for (size_t idx = 1; idx < neighbor_it.size(); ++idx) {
                auto &it = neighbor_it[idx], end = neighbor_end[idx];
                while (get_index(*it) > get_index(neighbor))
                    if (++it == end) return false;
                if (get_index(*it) != get_index(neighbor))
                    goto continue_outer;
                else
                    neighbor = std::max(neighbor, *it);
            }
            while (k > 0 && vertices[k - 1] > get_index(neighbor)) {
                if (!all_cofacets) return false;
                idx_below -= binomial_coeff(vertices[k - 1], k);
                idx_above += binomial_coeff(vertices[k - 1], k + 1);
                --k;
            }
            return true;
            continue_outer:;
        }
        return false;
    }

    diameter_entry_t next() {
        ++neighbor_it[0];
        value_t cofacet_diameter = std::max(get_diameter(simplex), get_diameter(neighbor));
        index_t cofacet_index = idx_above + binomial_coeff(get_index(neighbor), k + 1) + idx_below;
        coefficient_t cofacet_coefficient =
                (k & 1 ? modulus - 1 : 1) * get_coefficient(simplex) % modulus;
        return diameter_entry_t(cofacet_diameter, cofacet_index, cofacet_coefficient);
    }
};

#endif // SIMPLEX_COBOUNDARY_ENUMERATOR_TPP