#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/catch_message.hpp>

#include <sstream>

#include "index/index_flags.hpp"
#include "index/index_sync_mode.hpp"
#include "index_cmp.hpp"

using namespace lf;

void cmp_index_flags_byte(index_flags flags, std::uint8_t expected) {
    INFO("mode=" << flags.mode << ",sync=" << flags.sync);
    std::uint8_t& actual = reinterpret_cast<std::uint8_t&>(flags);
    REQUIRE( actual == expected );
}

TEST_CASE("set", "[index_flags]") {
    cmp_index_flags_byte(index_flags { .mode = index_sync_mode::NONE, .sync = false }, 0);
    cmp_index_flags_byte(index_flags { .mode = index_sync_mode::RECURSIVE, .sync = true }, 0b110);
    cmp_index_flags_byte(index_flags { .mode = index_sync_mode::SHALLOW, .sync = false }, 1);
}

TEST_CASE("serialization", "[index_flags]") {
    std::stringstream ss;

    const index_flags s = { 
        .mode = GENERATE( index_sync_mode::NONE, index_sync_mode::SHALLOW, index_sync_mode::RECURSIVE ), 
        .sync = GENERATE( false, true ) 
    };

    REQUIRE( (ss << with_ref_format<format::BINARY>(s)).good() );

    index_flags d;
    REQUIRE( (ss >> with_ref_format<format::BINARY>(d)).good() );

    cmp_index_flags(s, d);
}
