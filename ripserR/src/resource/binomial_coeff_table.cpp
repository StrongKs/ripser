//
// Created by Kent Phipps on 2/16/25.
//

#include "binomial_coeff_table.h"

// Function to check overflow (define as needed)
void check_overflow(index_t value) {
    // Implement overflow check logic if required
}

binomial_coeff_table::binomial_coeff_table(index_t n, index_t k)
        : B(k + 1, std::vector<index_t>(n + 1, 0)) {
    for (index_t i = 0; i <= n; ++i) {
        B[0][i] = 1;
        for (index_t j = 1; j < std::min(i, k + 1); ++j)
            B[j][i] = B[j - 1][i - 1] + B[j][i - 1];
        if (i <= k) B[i][i] = 1;
        check_overflow(B[std::min(i >> 1, k)][i]);
    }
}

index_t binomial_coeff_table::operator()(index_t n, index_t k) const {
    assert(k < B.size() && n < B[k].size() && n >= k - 1);
    return B[k][n];
}