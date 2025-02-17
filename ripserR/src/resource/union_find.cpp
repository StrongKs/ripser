//
// Created by Kent Phipps on 2/16/25.
//

#include "union_find.h"
#include "types.h"

// Constructor implementation
union_find::union_find(const index_t n) : parent(n), rank(n, 0) {
    for (index_t i = 0; i < n; ++i) {
        parent[i] = i;
    }
}

// Find function implementation with path compression
index_t union_find::find(index_t x) {
    index_t y = x, z;
    while ((z = parent[y]) != y) {
        y = z;
    }
    while ((z = parent[x]) != y) {
        parent[x] = y;
        x = z;
    }
    return y;
}

// Union by rank implementation
void union_find::link(index_t x, index_t y) {
    if ((x = find(x)) == (y = find(y))) {
        return;
    }
    if (rank[x] > rank[y]) {
        parent[y] = x;
    } else {
        parent[x] = y;
        if (rank[x] == rank[y]) {
            ++rank[y];
        }
    }
}