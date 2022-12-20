#include <catch2/catch_test_macros.hpp>

#include <fmt/core.h>

#include <util/map.hpp>

using namespace std::literals;

TEST_CASE("heterogenous_lookup", "[umap]") {
    leafsync::unordered_string_map<int> map;
    map["abc"] = 11;
    
    REQUIRE( map.contains("abc"sv) );
    REQUIRE_FALSE( map.contains("xyz"sv) );
}