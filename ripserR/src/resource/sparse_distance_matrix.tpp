#ifndef SPARSE_DISTANCE_MATRIX_TPP
#define SPARSE_DISTANCE_MATRIX_TPP

template <typename DistanceMatrix>
sparse_distance_matrix::sparse_distance_matrix(const DistanceMatrix& mat, const value_t threshold)
        : neighbors(mat.size()), num_edges(0) {

    for (size_t i = 0; i < size(); ++i) {
        for (size_t j = 0; j < size(); ++j) {
            if (i != j) {
                auto d = mat(i, j);
                if (d <= threshold) {
                    ++num_edges;
                    neighbors[i].push_back({j, d});
                }
            }
        }
    }
}

#endif // SPARSE_DISTANCE_MATRIX_TPP