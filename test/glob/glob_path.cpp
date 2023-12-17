#include <catch2/catch_test_macros.hpp>

#include "glob/glob_path.hpp"

using namespace lf;

namespace fs = std::filesystem;

TEST_CASE("glob_path: matches", "[glob_path]") {
    
    glob_path gp = { "a", globstar{}, "c.txt" };
    REQUIRE( gp.matches(fs::path("a") / "b" / "c.txt") );
    REQUIRE_FALSE( gp.matches(fs::path("a") / "b" / "d.txt") );
    REQUIRE( gp.matches(fs::path("a") / "b" / "e" / "x" / "c.txt") );

}
