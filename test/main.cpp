#include <catch2/catch_session.hpp>

#include "util/console.hpp"

int main(int argc, char* argv[]) {
	lf::console_init();
	return Catch::Session().run(argc, argv);
}