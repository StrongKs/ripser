//
// Created by Kent Phipps on 2/16/25.
//

#ifndef COMPRESSED_SPARSE_MATRIX_H
#define COMPRESSED_SPARSE_MATRIX_H

#include <vector>
#include <cassert>
#include "types.h"  // Assuming index_t is defined in this file

template <typename ValueType>
class compressed_sparse_matrix {
    std::vector<size_t> bounds;
    std::vector<ValueType> entries;

    using iterator = typename std::vector<ValueType>::iterator;
    using iterator_pair = std::pair<iterator, iterator>;

public:
    size_t size() const;

    iterator_pair subrange(const index_t index);

    void append_column();

    void push_back(const ValueType e);
};

// Include the template implementation file
#include "compressed_sparse_matrix.tpp"

#endif // COMPRESSED_SPARSE_MATRIX_H