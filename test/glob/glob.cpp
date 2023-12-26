#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <iostream>

#include "glob/glob.hpp"

using namespace lf;

TEST_CASE("glob: not matches", "[glob]") {
    REQUIRE_FALSE( glob { 
        glob::star {}, 
        "abc"
    }.matches("xaaabcaabcx") );
}

TEST_CASE("glob: match last sequence", "[glob]") {
    REQUIRE( glob { 
        glob::star {}, 
        "aa", 
    }.matches("xxxxaaa") );
}

TEST_CASE("glob: matches", "[glob]") {

    REQUIRE( glob { 
        glob::star {}, 
        "abc", 
        glob::any {} 
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

TEST_CASE("glob: match performance", "[glob]") {
    for (size_t iter = 0; iter < 40; iter++) {

        glob g;
        for (size_t i = 0; i < iter; i++) {
            g.elements.push_back("a");
            g.elements.push_back(glob::star());
        }
        g.elements.push_back("b");

        std::string str(iter, 'a');

        const auto t_start = std::chrono::high_resolution_clock::now();
        CHECK_FALSE( g.matches(str) );
        const auto t_end = std::chrono::high_resolution_clock::now();

        std::cout << iter <<  " stars: took "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start).count() 
            << "ns" << std::endl;
    
    }
}

TEST_CASE("glob: parse", "[glob]") {
    glob g = glob::parse("a*b?c[ab]d[!]e[!ab]x[a-zA-Z]");


}