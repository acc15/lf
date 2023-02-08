#include <catch2/catch_test_macros.hpp>

#include "util/string_map.hpp"

using namespace lf;

TEST_CASE("unordered_string_map put and find", "[string_map]") {
    
    unordered_string_map<int> m;

    m["abc"] = 10;

    auto it = m.find("abc");
    REQUIRE( it != m.end() );
    REQUIRE( it->first == "abc" );
    REQUIRE( it->second == 10 );

    const char* nt = "abc";
    std::string_view sv = "abc";
    std::string s = "abc";

    REQUIRE( m.find(nt) == it );
    REQUIRE( m.find(sv) == it );
    REQUIRE( m.find(s) == it );

}