//
// Created by Kent Phipps on 2/16/25.
//

#include "compressed_distance_matrix.h"

// Explicit template instantiations for commonly used types
template class compressed_distance_matrix<LOWER_TRIANGULAR>;
template class compressed_distance_matrix<UPPER_TRIANGULAR>;