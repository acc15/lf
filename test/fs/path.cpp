#include <catch2/catch_test_macros.hpp>

#include "fs/path.hpp"

#include "../test_util.hpp"

// namespace fs = std::filesystem;
using namespace lf;

TEST_CASE("is_subpath", "[path]") {
    REQUIRE( is_subpath("a/b/c", "a/b") );
    REQUIRE_FALSE( is_subpath("a/b/c", "b") );
    REQUIRE_FALSE( is_subpath("a", "a/b/c") );
    REQUIRE_FALSE( is_subpath(test_root / "a", "a") );
    REQUIRE( is_subpath(test_root / "a", test_root / "a") );
}

