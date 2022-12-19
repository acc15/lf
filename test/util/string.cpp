#include <catch2/catch_test_macros.hpp>

#include <util/string.hpp>

TEST_CASE("rtrim", "[string]") {
	REQUIRE(leafsync::rtrim("  abc   ") == "  abc");
	REQUIRE(leafsync::rtrim("abc") == "abc");
	REQUIRE(leafsync::rtrim("  юникод  ") == "  юникод");
}

TEST_CASE("ltrim", "[string]") {
	REQUIRE(leafsync::ltrim("  abc   ") == "abc   ");
	REQUIRE(leafsync::ltrim("abc ") == "abc ");
	REQUIRE(leafsync::ltrim("  юникод") == "юникод");
}

TEST_CASE("trim", "[string]") {
	REQUIRE(leafsync::trim("  abc   ") == "abc");
	REQUIRE(leafsync::trim("  юникод  ") == "юникод");
}