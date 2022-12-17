#include <catch2/catch_test_macros.hpp>

#include <fstream>
#include <fmt/core.h>

void load(std::istream& stream) {

    // std::string line;
    // while (std::getline(stream, line)) {
    //     fmt::print("Line read: {}\n", line);
    // }

}

TEST_CASE( "load", "[config]" ) {
    std::ifstream f("../test/config/config.txt");
    load(f);
}