#include <catch2/catch_test_macros.hpp>

#include "fs/util.hpp"

#include "test_util.hpp"

using namespace lf;
namespace fs = std::filesystem;

TEST_CASE("is_subpath", "[path]") {
    REQUIRE( is_subpath("a/b/c", "a/b") );
    REQUIRE_FALSE( is_subpath("a/b/c", "b") );
    REQUIRE_FALSE( is_subpath("a", "a/b/c") );
    REQUIRE_FALSE( is_subpath(test_root_path / "a", "a") );
    REQUIRE( is_subpath(test_root_path / "a", test_root_path / "a") );
}

TEST_CASE("create_parent_dirs", "[path]") {

    fs::path p1 = "abc";
    REQUIRE_FALSE( p1.has_parent_path() );
    REQUIRE( p1.parent_path().empty() );
    REQUIRE_FALSE( lf::create_parent_dirs(p1.parent_path()) );

    fs::path p2 = p1 / "xyz";
    REQUIRE( p2.has_parent_path() );

}

TEST_CASE("join_path", "[path]") {

    fs::path p1 = "abc";
    fs::path empty;

    REQUIRE_FALSE( (p1 / empty) == p1 );
    REQUIRE( join_path(p1, empty) == p1 );

}
