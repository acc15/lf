#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <string>
#include <unordered_set>

#include "index_cmp.hpp"

namespace lf {

    void cmp_index_flags(index_flags l, index_flags r) {
        {
            INFO("l.mode=" << l.mode << ", r.mode=" << r.mode);
            CHECK( (l.mode == r.mode) );
        }
        {
            INFO("l.sync=" << l.sync << ", r.sync=" << r.sync);
            CHECK( (l.sync == r.sync) );
        }
    }

    void cmp_index_entry(const index_entry& l, const index_entry& r) {
        cmp_index_flags(l.flags, r.flags);

        const auto& lmap = l.entries, rmap = r.entries;
        CHECK( lmap.size() == rmap.size() );
        for (const auto& le: l.entries) {
            INFO("entry=" << le.first);
            CHECK( rmap.contains(le.first) );

            const auto rit = rmap.find(le.first);
            if (rit != rmap.end()) {
                cmp_index_entry(le.second, rit->second);
            }
        }

        for (const auto& re: r.entries) {
            INFO("entry=" << re.first);
            CHECK( lmap.contains(re.first) );
        }

    }

}