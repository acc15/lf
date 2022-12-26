#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <sstream>

#include "io/log.hpp"
#include "io/null_stream.hpp"

using namespace lf;

TEST_CASE("log stream", "[log]") {
    log.min_level = log::INFO;

    log.info();
    CHECK(&log() == &std::cout);
    
    log.error();
    CHECK(&log() == &std::cerr);

    log.min_level = log::MUTE;
    CHECK(&log() == &nullout);
}

TEST_CASE("stream override", "[log]") {
    
}


