cmake_minimum_required(VERSION 3.27)
project(RipserV2)

set(CMAKE_CXX_STANDARD 14)

include_directories(ripserR/src/resource)

add_executable(RipserV2
#        ripserR/src/resource/compressed_sparse_matrix.h
#        ripserR/src/resource/compressed_sparse_matrix.tpp
#        ripserR/src/resource/ripser.cpp
#        ripserR/src/resource/ripser.h
#        ripserR/src/resource/ripser.tpp
#        ripserR/src/resource/simplex_coboundary_enumerator.tpp
#        ripserR/src/resource/sparse_distance_matrix.cpp
#        ripserR/src/resource/sparse_distance_matrix.h
#        ripserR/src/resource/sparse_distance_matrix.tpp
#        ripserR/src/resource/types.h
#        ripserR/src/resource/union_find.cpp
#        ripserR/src/resource/union_find.h
#        ripserR/src/main.cpp
#        ripserR/src/rcpp_hello_world.cpp
#        ripserR/src/RcppExports.cpp
        ripserR/src/ripserSol.cpp
#        ripserR/src/ripserSolution.cpp
#        ripserR/src/resource/compressed_distance_matrix.h
#        ripserR/src/resource/compressed_distance_matrix.tpp
#        ripserR/src/resource/binomial_coeff_table.cpp
)
