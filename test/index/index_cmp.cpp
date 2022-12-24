#include <catch2/catch_test_macros.hpp>

#include "index_cmp.hpp"

namespace lf {

    void cmp_index_flags(index_flags l, index_flags r) {
        CHECK( (l.mode == r.mode) );
        CHECK( (l.sync == r.sync) );
    }

    void cmp_index_entry(const index_entry& l, const index_entry& r);

    void cmp_index_entries(const std::vector<index_entry>& l, const std::vector<index_entry>& r) {
        CHECK( l.size() == r.size() );
        for (size_t i = 0; i < std::min(l.size(), r.size()); i++) {
            cmp_index_entry(l[i], r[i]);
        }
    }

    void cmp_index_entry(const index_entry& l, const index_entry& r) {
        CHECK( l.name == r.name );
        cmp_index_flags(l.flags, r.flags);
        cmp_index_entries(l.entries, r.entries);
    }

    void cmp_index(const index_root& l, const index_root& r) {
        cmp_index_flags(l.flags, r.flags);
        cmp_index_entries(l.entries, r.entries);
    }

}