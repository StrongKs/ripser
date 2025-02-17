//
// Created by Kent Phipps on 2/16/25.
//

#ifndef BINOMIAL_COEFF_TABLE_H
#define BINOMIAL_COEFF_TABLE_H

#include <vector>
#include <cassert>
#include <algorithm>
#include "types.h"  // Ensure index_t is properly defined

class binomial_coeff_table {
private:
    std::vector<std::vector<index_t>> B;

public:
    binomial_coeff_table(index_t n, index_t k);

    index_t operator()(index_t n, index_t k) const;
};

#endif // BINOMIAL_COEFF_TABLE_H