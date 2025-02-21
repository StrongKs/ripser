//
// Created by Kent Phipps on 2/16/25.
//

#pragma once

#include <iostream>
#include <vector>
#include "ripserSol.cpp"
//#include "resource/sparse_distance_matrix.h"
//#include "resource/compressed_distance_matrix.h"

using namespace std;

int main() {
    cout << "Testing Ripser Implementation..." << endl;

    // Define a small example distance matrix
    vector<vector<value_t>> distance_matrix = {
            {0, 1.0, 2.0},
            {1.0, 0, 1.5},
            {2.0, 1.5, 0}
    };

    vector<value_t> dis_vector = {4.0f,
                                  3.0f, 5.0f,
                                  5.0f, 3.0f, 4.0f};

    // Convert to a compressed lower distance matrix
    compressed_lower_distance_matrix(std::move(dis_vector));
    cout << "hello" << endl;
//    compressed_lower_distance_matrix comp_dist(dis_vector);

//    // Instantiate and test Ripser with compressed lower distance matrix
//    ripser<compressed_lower_distance_matrix> ripser_comp(std::move(comp_dist), 2, 2.0, 1.0, 2);
//    cout << "Computing barcodes for compressed lower distance matrix..." << endl;
//    ripser_comp.compute_barcodes();
//
//    // Convert to a sparse distance matrix
//    sparse_distance_matrix sparse_dist(distance_matrix, 2.0);
//
//    // Instantiate and test Ripser with sparse distance matrix
//    ripser<sparse_distance_matrix> ripser_sparse(std::move(sparse_dist), 2, 2.0, 1.0, 2);
//    cout << "Computing barcodes for sparse distance matrix..." << endl;
//    ripser_sparse.compute_barcodes();
//
//    cout << "Testing completed!" << endl;
    return 0;
}