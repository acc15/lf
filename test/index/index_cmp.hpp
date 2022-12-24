#pragma once

#include <vector>

#include <index/index_root.hpp>

namespace lf {

    void cmp_index_flags(index_flags l, index_flags r);
    void cmp_index_entry(const index_entry& l, const index_entry& r);
    void cmp_index_entries(const std::vector<index_entry>& l, const std::vector<index_entry>& r);
    void cmp_index(const index_root& l, const index_root& r);

}