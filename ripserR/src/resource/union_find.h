//
// Created by Kent Phipps on 2/16/25.
//

#ifndef RIPSERV2_UNION_FIND_H
#define RIPSERV2_UNION_FIND_H

#include "ripser.h"
#include "types.h"

using namespace std;

class union_find {
    vector<index_t> parent;
    vector<uint8_t> rank;

public:
    explicit union_find(const index_t n);

    index_t find(index_t x);
    void link(index_t x, index_t y);
};


#endif //RIPSERV2_UNION_FIND_H
