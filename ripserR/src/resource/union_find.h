//
// Created by Kent Phipps on 2/16/25.
//

#ifndef RIPSERV2_UNION_FIND_H
#define RIPSERV2_UNION_FIND_H

#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "types.h"

#include <vector>
#include <cstdint>

class union_find {
private:
    std::vector<index_t> parent;
    std::vector<uint8_t> rank;

public:
    explicit union_find(index_t n);
    index_t find(index_t x);
    void link(index_t x, index_t y);
};

#endif // UNION_FIND_H

#endif //RIPSERV2_UNION_FIND_H
