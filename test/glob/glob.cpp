#include <catch2/catch_test_macros.hpp>

#include "glob/glob.hpp"

using namespace lf;

TEST_CASE("glob: matches", "[glob]") {

    REQUIRE( glob { 
        glob::star {}, 
        "abc", 
        glob::any {} 
    }.matches("xaaabcaabcx") );

    REQUIRE_FALSE( glob { 
        glob::star {}, 
        "abc"
    }.matches("xaaabcaabcx") );

    glob range_glob = {
        glob::range { {U'A', U'Z'}, {U'a', U'z'} },
        glob::range { {U'0', U'9'} }
    };

    REQUIRE(range_glob.matches("A1"));
    REQUIRE(range_glob.matches("a9"));
    REQUIRE(range_glob.matches("z2"));
    REQUIRE(range_glob.matches("Z3"));
    REQUIRE_FALSE(range_glob.matches("0Z"));

    glob ext_glob = {
        glob::star {},
        ".txt"
    };

    REQUIRE(ext_glob.matches("a.txt"));
    REQUIRE(ext_glob.matches("xyz.txt"));
    REQUIRE(ext_glob.matches(".txt"));
    REQUIRE(ext_glob.matches("Hello.txt"));
    REQUIRE_FALSE(ext_glob.matches("Hello.zip"));

}

