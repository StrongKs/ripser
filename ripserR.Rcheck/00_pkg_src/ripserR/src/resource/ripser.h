//
// Created by Kent Phipps on 2/16/25.
//

#ifndef RIPSERR_RIPSER_H
#define RIPSERR_RIPSER_H



#include <vector>
#include <iostream>
#include <algorithm>

#include "sparse_matrix.h"
#include "types.h"



// Ripser class declaration
class ripser {
private:
    const sparse_distance_matrix dist;
    const index_t n, dim_max;
    const value_t threshold;
    const coefficient_t modulus;

public:
    ripser(sparse_distance_matrix&& _dist, index_t _dim_max, value_t _threshold, coefficient_t _modulus);

    void compute_barcodes();

    std::vector<std::pair<value_t, value_t>> get_persistence_pairs();
};

#endif //RIPSERR_RIPSER_H
