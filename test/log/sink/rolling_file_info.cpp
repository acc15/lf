#include <catch2/catch_test_macros.hpp>

#include <algorithm>

#include "log/sink/rolling_file_info.hpp"
#include "test_util.hpp"

namespace fs = std::filesystem;

using namespace lf;

TEST_CASE("rolling_file_info make_path", "[rolling_file_info]") {
    rolling_file_info fi = { .time = fs::file_time_type{}, .name = ".2022_01_01.", .seq = 10 };
    REQUIRE( fi.make_path(fs::path("abc.log")) == fs::path("abc.log.2022_01_01.10") );
    REQUIRE( fi.make_path(test_root_path / "abc.log") == test_root_path / "abc.log.2022_01_01.10" );
}

TEST_CASE("rolling_file_info parse", "[rolling_file_info]") {

    std::optional<rolling_file_info> a = rolling_file_info::parse("abc.log.2022_01_01.9", "abc.log");
    REQUIRE( a.has_value() );
    REQUIRE( a.value() == rolling_file_info { .time = {}, .name = ".2022_01_01.", .seq = 9 } );

    std::optional<rolling_file_info> b = rolling_file_info::parse("abc.log.2022_01_01.9", "xyz.log");
    REQUIRE_FALSE( b.has_value() );

    std::optional<rolling_file_info> c = rolling_file_info::parse("abc.log.9", "abc.log");
    REQUIRE( c.has_value() );
    REQUIRE( c.value() == rolling_file_info { .time = {}, .name = ".", .seq = 9 } );

}


TEST_CASE("rolling_file_info compare", "[rolling_file_info]") {

    rolling_file_info a = { .time = fs::file_time_type { fs::file_time_type::duration(100) }, .name = ".2024_03_01.", .seq = 20 };
    rolling_file_info b = { .time = fs::file_time_type { fs::file_time_type::duration(200) }, .name = ".2024_01_30.", .seq = 9 };
    rolling_file_info c = { .time = fs::file_time_type { fs::file_time_type::duration(200) }, .name = ".2024_01_30.", .seq = 10 };
    rolling_file_info d = { .time = fs::file_time_type { fs::file_time_type::duration(200) }, .name = ".2024_02_02.", .seq = 1 };

    REQUIRE( a < b );
    REQUIRE( b < c );
    REQUIRE( c < d );
    REQUIRE( a < d );

    std::vector<rolling_file_info> v = { c, b, d, a };

    std::sort(v.begin(), v.end());

    REQUIRE( v == std::vector<rolling_file_info> { a, b, c, d } );

}