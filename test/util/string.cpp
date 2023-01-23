#include <catch2/catch_test_macros.hpp>

#include "util/string.hpp"

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

TEST_CASE("lower inplace", "[string]") {
	std::string str = "юtrAceд";
	lower(str);
	REQUIRE( str == "юtraceд");
}

TEST_CASE("upper inplace", "[string]") {
	std::string str = "юtrAceд";
	upper(str);
	REQUIRE( str == "юTRACEд" );
}

TEST_CASE("ci_str_eq", "[string]") {
	REQUIRE( ci_str_eq("aBc", "ABC") );
	REQUIRE( ci_str_eq("ABC", "abc") );
	REQUIRE_FALSE( ci_str_eq("aBc", "cde") );
	REQUIRE_FALSE( ci_str_eq("aBc", "a") );
	REQUIRE_FALSE( ci_str_eq("aBc", "ab") );
}