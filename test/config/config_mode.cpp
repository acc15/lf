#include <catch2/catch_test_macros.hpp>

#include <config/config_mode.hpp>

using namespace lf;

TEST_CASE("should parse", "[config_mode]") {
	config_mode mode;
	REQUIRE((parse_config_mode("r", mode) && mode == config_mode::READ));
	REQUIRE((parse_config_mode("READ", mode) && mode == config_mode::READ));
	REQUIRE((parse_config_mode("w", mode) && mode == config_mode::WRITE));
	REQUIRE((parse_config_mode("WRITE", mode) && mode == config_mode::WRITE));
	REQUIRE_FALSE( parse_config_mode("x", mode) );
}