#include <catch2/catch_test_macros.hpp>

#include "util/pick.hpp"

enum class test_enum {
    HELLO,
    WORLD,
    DEFAULT
};

const char* test_enum_names[] = { "hello", "WORLD", "unknown" };

TEST_CASE("util: parse_enum", "[pick]") {
    REQUIRE( lf::parse_enum<test_enum>(" hElLo  ", test_enum::DEFAULT, test_enum_names) == test_enum::HELLO );
    REQUIRE( lf::parse_enum<test_enum>("world ", test_enum::DEFAULT, test_enum_names) == test_enum::WORLD );
    REQUIRE( lf::parse_enum<test_enum>("  unknown", test_enum::DEFAULT, test_enum_names) == test_enum::DEFAULT );
    REQUIRE( lf::parse_enum<test_enum>("xyz", test_enum::DEFAULT, test_enum_names) == test_enum::DEFAULT );
    REQUIRE( lf::parse_enum<test_enum>(" 0 ", test_enum::DEFAULT, test_enum_names) == test_enum::HELLO );
    REQUIRE( lf::parse_enum<test_enum>(" 1 ", test_enum::DEFAULT, test_enum_names) == test_enum::WORLD );
    REQUIRE( lf::parse_enum<test_enum>("2", test_enum::DEFAULT, test_enum_names) == test_enum::DEFAULT );
    REQUIRE( lf::parse_enum<test_enum>("3", test_enum::DEFAULT, test_enum_names) == test_enum::DEFAULT );
}