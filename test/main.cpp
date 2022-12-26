#include <catch2/catch_session.hpp>

#include "io/log.hpp"
#include "util/console.hpp"

int main(int argc, char* argv[]) {
	lf::log.min_level = lf::log::MUTE;
	lf::console_init();
	return Catch::Session().run(argc, argv);
}