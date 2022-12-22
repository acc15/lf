#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fstream>
#include <sstream>

#include <fmt/core.h>

#include <index/index.hpp>

using namespace leafsync;

TEST_CASE("set", "[index_flags]") {
    index_flags flags = {
        .mode = index_sync_mode::RECURSIVE,
        .sync = true
    };
    std::uint8_t& byte = reinterpret_cast<std::uint8_t&>( flags );
    REQUIRE( byte == 0b110 );
}

TEST_CASE("serialization", "[index_flags]") {
    std::stringstream ss;

    index_flags s = { 
        .mode = GENERATE( index_sync_mode::NONE, index_sync_mode::SHALLOW, index_sync_mode::RECURSIVE ), 
        .sync = GENERATE( false, true ) 
    };

    REQUIRE( (ss << s).good() );

    index_flags d;
    REQUIRE( (ss >> d).good() );

    REQUIRE( (d.mode == s.mode) );
    REQUIRE( (d.sync == s.sync) );
}

