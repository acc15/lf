#include <catch2/catch_test_macros.hpp>

#include "fs/util.hpp"

#include "test_util.hpp"

using namespace lf;
namespace fs = std::filesystem;

TEST_CASE("path: absolute_path", "[path]") {
    const auto cd = fs::current_path();
    const auto this_dir = cd.filename().string();

    REQUIRE( absolute_path("") == cd );
    REQUIRE( absolute_path(".") == cd );
    REQUIRE( absolute_path("./") == cd );
    REQUIRE( absolute_path("../") == cd.parent_path() );
    REQUIRE( absolute_path("../a.txt") == cd.parent_path() / "a.txt" );
    REQUIRE( absolute_path("./a.txt") == cd / "a.txt" );
    REQUIRE( absolute_path("./a.txt") == cd / "a.txt" );
    REQUIRE( absolute_path(fs::path("..") / this_dir / "a.txt") == cd / "a.txt" );
}

TEST_CASE("path: relative_path", "[path]") {
    const fs::path root_path = fs::path("a");
    const fs::path test_path = fs::path("b") / "c";

    REQUIRE( relative_path("", "") == "" );
    REQUIRE( relative_path(root_path / test_path, root_path) == test_path );
    REQUIRE_FALSE( relative_path(test_path, fs::current_path()) );
    REQUIRE_FALSE( relative_path(test_path, fs::path("x") / "y" / "z") );
}

TEST_CASE("path: is_subpath", "[path]") {
    REQUIRE( is_subpath("a/b/c", "a/b") );
    REQUIRE_FALSE( is_subpath("a/b/c", "b") );
    REQUIRE_FALSE( is_subpath("a", "a/b/c") );
    REQUIRE_FALSE( is_subpath(test_root_path / "a", "a") );
    REQUIRE( is_subpath(test_root_path / "a", test_root_path / "a") );
}

TEST_CASE("path: create_parent_dirs", "[path]") {
    fs::path p1 = "abc";
    REQUIRE_FALSE( p1.has_parent_path() );
    REQUIRE( p1.parent_path().empty() );
    REQUIRE_FALSE( lf::create_parent_dirs(p1.parent_path()) );

    fs::path p2 = p1 / "xyz";
    REQUIRE( p2.has_parent_path() );
}

TEST_CASE("path: join_path", "[path]") {
    REQUIRE( join_path("abc", fs::path()) == fs::path("abc") );
    REQUIRE( join_path("abc", "xyz") == fs::path("abc") / "xyz" );
}
