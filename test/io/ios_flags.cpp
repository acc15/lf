#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include "io/ios_flags.hpp"

TEST_CASE("test", "[ios_flags]") {
    
    std::stringstream s;

    std::ios_base::openmode mode = std::ios_base::out | std::ios_base::trunc | std::ios_base::binary;
    s << lf::with_cref_format<lf::format::TEXT>(mode);
    REQUIRE( s.str() == "out | binary | truncate");

}