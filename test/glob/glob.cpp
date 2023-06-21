#include <catch2/catch_test_macros.hpp>

#include "glob/glob.hpp"
#include <encoding/utf8.hpp>

#include <sstream>

#include "../test_util.hpp"
#include "match_tester.hpp"

using namespace lf;

using g = glob<utf8_encoding>;

TEST_CASE("glob: matches", "[glob]") {

    g glob(make_unique_container<g::matcher_vector>(
        std::make_unique<g::char_matcher>(true),
        std::make_unique<g::char_matcher>(false, g::char_matcher::map_type {{U'A', U'Z'}}),
        std::make_unique<g::star_matcher>(),
        std::make_unique<g::string_matcher>(".txt")
    ));

    match_tester t1("ЭZany12.txt");
    REQUIRE( glob.matches(t1.buf) );
    REQUIRE( t1.pos() == t1.size() );

    match_tester t2("ZnotnextZ.txt");
    REQUIRE_FALSE( glob.matches(t2.buf) );
    REQUIRE( t2.pos() == 2 );

}