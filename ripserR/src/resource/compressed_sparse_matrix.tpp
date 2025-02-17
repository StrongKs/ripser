#ifndef COMPRESSED_SPARSE_MATRIX_TPP
#define COMPRESSED_SPARSE_MATRIX_TPP

#include "compressed_sparse_matrix.h"

// Returns the number of columns (size of bounds)
template <typename ValueType>
size_t compressed_sparse_matrix<ValueType>::size() const {
    return bounds.size();
}

// Returns the range of entries for a given column index
template <typename ValueType>
typename compressed_sparse_matrix<ValueType>::iterator_pair
compressed_sparse_matrix<ValueType>::subrange(const index_t index) {
    return {entries.begin() + (index == 0 ? 0 : bounds[index - 1]),
            entries.begin() + bounds[index]};
}

// Appends a new column to the matrix
template <typename ValueType>
void compressed_sparse_matrix<ValueType>::append_column() {
    bounds.push_back(entries.size());
}

// Pushes a new value to the last column
template <typename ValueType>
void compressed_sparse_matrix<ValueType>::push_back(const ValueType e) {
    assert(0 < size());  // Ensure there's at least one column before adding
    entries.push_back(e);
    ++bounds.back();
}

#endif // COMPRESSED_SPARSE_MATRIX_TPP
