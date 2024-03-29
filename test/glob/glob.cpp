#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <iostream>

#include "glob/glob.hpp"

using namespace lf;

void test_range(const glob::range& actual, const glob::range& expected) {
    REQUIRE( actual.map == expected.map );
    REQUIRE( actual.inverse == expected.inverse );
}

void test_element(const glob::element& actual, const glob::element& expected) {
    REQUIRE( actual.index() == expected.index() );
    std::visit([&expected](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            REQUIRE( v == std::get<std::string>(expected) );
        } else if constexpr (std::is_same_v<T, glob::range>) {
            test_range(v, std::get<glob::range>(expected));
        }
    }, actual);
}

void test_elements(const glob::element_vector& actual, const glob::element_vector& expected) {
    CHECK(actual.size() == expected.size());
    for (size_t i = 0; i < std::min(actual.size(), expected.size()); i++) {
        INFO("at " << i << " element");
        test_element(actual[i], expected[i]);
    }
}

void test_parse(std::string_view expr, const glob::element_vector& expected) {
    INFO("parsing glob: " << expr);
    test_elements(glob::parse(expr).elements, expected);
}

TEST_CASE("glob: not matches", "[glob][match]") {
    REQUIRE_FALSE( glob { glob::star {},  "abc"}.matches("xaaabcaabcx") );
}

TEST_CASE("glob: match last sequence", "[glob][match]") {
    REQUIRE( glob { glob::star {}, "aa", }.matches("xxxxaaa") );
}

TEST_CASE("glob: match last any", "[glob][match]") {
    REQUIRE( glob { glob::star(), "abc", glob::any() }.matches("xaaabcaabcx") );
}

TEST_CASE("glob: match range", "[glob][match]") {
    glob range_glob = { glob::range{ {{U'A', U'Z'}, {U'a', U'z'}} }, glob::range{ {{U'0', U'9'}} } };
    REQUIRE(range_glob.matches("A1"));
    REQUIRE(range_glob.matches("a9"));
    REQUIRE(range_glob.matches("z2"));
    REQUIRE(range_glob.matches("Z3"));
    REQUIRE_FALSE(range_glob.matches("0Z"));
}

TEST_CASE("glob: match utf8", "[glob][match]") {
    glob glob = { glob::range{ {{U'а', U'я'}} } };
    REQUIRE( glob.matches("а") );
    REQUIRE( glob.matches("б") );
    REQUIRE( glob.matches("в") );
    REQUIRE( glob.matches("я") );
    REQUIRE( !glob.matches("А") );
}

TEST_CASE("glob: match extension", "[glob][match]") {
    glob ext_glob = { glob::star(), ".txt" };
    REQUIRE(ext_glob.matches("a.txt"));
    REQUIRE(ext_glob.matches("xyz.txt"));
    REQUIRE(ext_glob.matches(".txt"));
    REQUIRE(ext_glob.matches("Hello.txt"));
    REQUIRE_FALSE(ext_glob.matches("Hello.zip"));
}

TEST_CASE("glob: match performance", "[glob][match][!benchmark]") {
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

TEST_CASE("glob: parse general", "[glob][parse]") {
    test_parse("a*b?c[ab]d[!]e[!ab]x[a-zA-Z]", {
        "a",
        glob::star(),
        "b",
        glob::any(),
        "c",
        glob::range { {{U'a', U'b'}} },
        "d!e",
        glob::range { {{U'a', U'b'}}, true },
        "x",
        glob::range { {{U'a', U'z'}, {U'A', U'Z'}} }
    });
}

TEST_CASE("glob: parse range escape open bracket", "[glob][parse]") {
    test_parse("[[]", { "[" });
}

TEST_CASE("glob: parse range escape close bracket", "[glob][parse]") {
    test_parse("[]]", { "]" });
}

TEST_CASE("glob: parse range single char ", "[glob][parse]") {
    test_parse("[a-a][b-b]", { "ab" });
}

TEST_CASE("glob: parse range exclamation mark ", "[glob][parse]") {
    test_parse("[!]", { "!" });
}

TEST_CASE("glob: parse range join", "[glob][parse]") {
    test_parse("[c-ae-fd]", { glob::range { {{U'a', U'f'}} } });
}

TEST_CASE("glob: parse range enum", "[glob][parse]") {
    test_parse("[abcde]", { glob::range { {{U'a', U'e'}} } });
}

TEST_CASE("glob: parse range dash at end", "[glob][parse]") {
    test_parse("[c-]", { glob::range { {{U'c', U'c'}, {U'-', U'-'}} } });
}

TEST_CASE("glob: parse range single dash", "[glob][parse]") {
    test_parse("[-]", { "-" });
}

TEST_CASE("glob: parse multiple consecutive stars", "[glob][parse]") {
    test_parse("*****", { glob::star() });
}

TEST_CASE("glob: stringify", "[glob][stringify]") {
    REQUIRE( glob { 
        glob::star(), 
        glob::any(), 
        glob::range { {{U'А', U'Я'}, {U'а', U'я'}, {U'a', U'a'}} },
        glob::range { {{U'a', U'c'}}, true },
        "abc*[]?", 
    }.stringify() == "*?[aА-Яа-я][!a-c]abc[*][[][]][?]" );
}