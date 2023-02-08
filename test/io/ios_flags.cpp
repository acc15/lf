#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include "io/ios_flags.hpp"

TEST_CASE("ios_flags print", "[ios_flags]") {
    
    std::stringstream s;

    std::ios_base::openmode mode = std::ios_base::out | std::ios_base::trunc | std::ios_base::binary;
    s << lf::write_as<lf::ios_flags_format>(mode);
    REQUIRE( s.str() == "out|binary|trunc");

}