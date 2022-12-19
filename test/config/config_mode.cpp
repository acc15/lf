#include <catch2/catch_test_macros.hpp>

#include <config/config_mode.hpp>

using namespace leafsync;

TEST_CASE("should parse", "[config_mode]") {
	REQUIRE( parse_config_mode('r') == config_mode::READ );
	REQUIRE( parse_config_mode('w') == config_mode::WRITE );
	REQUIRE( parse_config_mode('x') == config_mode::UNKNOWN );
}