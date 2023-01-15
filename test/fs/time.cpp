#include <catch2/catch_test_macros.hpp>

#include "fs/time.hpp"

namespace fs = std::filesystem;

fs::file_time_type tp_from_int(const fs::file_time_type::duration::rep& r) {
    return fs::file_time_type(fs::file_time_type::duration(r));
}

TEST_CASE("last_write_time", "[time]") {
    
    fs::file_time_type t1 = tp_from_int(123424231423423L);
    fs::file_time_type expected_t1 = tp_from_int(123424231423400L);
    fs::file_time_type actual_t1 = lf::trunc_last_write_time(t1);
    REQUIRE( actual_t1 == expected_t1 );

    fs::file_time_type t2 = tp_from_int(-4767892066937254531L);
    fs::file_time_type expected_t2 = tp_from_int(-4767892066937254600L);
    fs::file_time_type actual_t2 = lf::trunc_last_write_time(t2);
    REQUIRE( actual_t2 == expected_t2 );

}