#include <catch2/catch_test_macros.hpp>

#include <util/string.hpp>

using namespace lf;

TEST_CASE("rtrim", "[string]") {
	REQUIRE(rtrim("  abc   ") == "  abc");
	REQUIRE(rtrim("abc") == "abc");
	REQUIRE(rtrim("  юникод  ") == "  юникод");
}

TEST_CASE("ltrim", "[string]") {
	REQUIRE(ltrim("  abc   ") == "abc   ");
	REQUIRE(ltrim("abc ") == "abc ");
	REQUIRE(ltrim("  юникод") == "юникод");
}

TEST_CASE("trim", "[string]") {
	REQUIRE(trim("  abc   ") == "abc");
	REQUIRE(trim("  юникод  ") == "юникод");
}

TEST_CASE("lower", "[string]") {
	REQUIRE(lower(" aBcюникод ") == " abcюникод ");
	REQUIRE(lower("  ЮНИКОД  ") == "  ЮНИКОД  ");
	REQUIRE(lower("ABCD") == "abcd");
}

TEST_CASE("upper", "[string]") {
	REQUIRE(upper(" aBcюникод ") == " ABCюникод ");
	REQUIRE(upper("  юникод  ") == "  юникод  ");
	REQUIRE(upper("abcD") == "ABCD");
}