#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <sstream>

#include "io/log.hpp"
#include "io/null_stream.hpp"


TEST_CASE("log stream", "[log]") {
    lf::log.min_level = lf::log::INFO;

    lf::log.info();
    CHECK(&lf::log() == &std::cout);
    
    lf::log.error();
    CHECK(&lf::log() == &std::cerr);

    lf::log.min_level = lf::log::MUTE;
    CHECK(&lf::log() == &lf::nullout);
}

TEST_CASE("stream override", "[log]") {
    std::stringstream test_stream;
    lf::log.stream = &test_stream;
    lf::log() << "abc" << std::endl;
    lf::log.stream = nullptr;

    REQUIRE(test_stream.str() == "abc\n");
}


