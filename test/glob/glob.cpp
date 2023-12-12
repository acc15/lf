#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "glob/glob.hpp"
#include "glob/matcher/any_matcher.hpp"
#include "glob/matcher/range_matcher.hpp"
#include "glob/matcher/star_matcher.hpp"
#include "glob/matcher/string_matcher.hpp"
#include "test_util.hpp"

using namespace lf;

class match_tester {
public:
    std::stringstream stream;
    std::streambuf* buf;

    match_tester(const std::string& string):
        stream(string),
        buf(stream.rdbuf())
    {}

    std::size_t pos() const {
        return static_cast<std::size_t>(buf->pubseekoff(0, std::ios_base::cur, std::ios_base::in));
    }

    std::size_t size() const {
        return stream.view().size();
    }

};

TEST_CASE("glob: matches", "[glob]") {

    glob g(make_unique_container<glob::matcher_vector>(
        std::make_unique<any_matcher>(),
        std::make_unique<range_matcher>(range_matcher::range_map {{U'A', U'Z'}}, false), // A-Z
        std::make_unique<star_matcher>(), // zero or more chars
        std::make_unique<string_matcher>(".txt") // .txt
    ));

    match_tester t1("ЭZany12.txt");
    REQUIRE( g.matches(t1.buf) );
    REQUIRE( t1.pos() == t1.size() );

    match_tester t2("ZnotnextZ.txt");
    REQUIRE_FALSE( g.matches(t2.buf) );
    REQUIRE( t2.pos() == 2 );

}