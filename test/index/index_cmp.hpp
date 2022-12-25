#pragma once

#include <vector>

#include "index/index_entry.hpp"

namespace lf {

    void cmp_index_flags(index_flags l, index_flags r);
    void cmp_index_entry(const index_entry& l, const index_entry& r);

}