#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include "io/openmode.hpp"

TEST_CASE("openmode print", "[openmode]") {
    
    std::stringstream s;

    std::ios_base::openmode mode = std::ios_base::out | std::ios_base::trunc | std::ios_base::binary;
    s << lf::write_as<lf::openmode_format>(mode);
    REQUIRE( s.str() == "out|binary|trunc");

}

TEST_CASE("default modes", "[openmode]") {
    
    REQUIRE( lf::default_openmode<std::istream>::default_mode == std::ios_base::in );
    REQUIRE( lf::default_openmode<std::istream>::force_mode == std::ios_base::in );
    REQUIRE( lf::default_openmode<std::ostream>::default_mode == std::ios_base::out );
    REQUIRE( lf::default_openmode<std::ostream>::force_mode == std::ios_base::out );
    REQUIRE( lf::default_openmode<std::iostream>::default_mode == 0 );
    REQUIRE( lf::default_openmode<std::iostream>::force_mode == 0 );

}