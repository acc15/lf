#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include <config/config.hpp>

using namespace leafsync;

TEST_CASE("load", "[config]") {

	config cfg;

	std::stringstream ss(
		"  # comment \n"
		"    \n"
		"  line\n");

	cfg.load(ss);

}