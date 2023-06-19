#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <glob/glob.hpp>
#include <encoding/utf8.hpp>

#include "../test_util.hpp"

using namespace lf;

using g = glob<utf8_encoding>;

class match_tester {
public:

    using stringstream = std::basic_stringstream<g::char_type, g::char_traits>;
    
    stringstream stream;
    g::streambuf* buf;

    match_tester(const g::string& string): stream(string), buf(stream.rdbuf()) {
    }

    std::size_t pos() const {
        return static_cast<std::size_t>(buf->pubseekoff(0, std::ios_base::cur, std::ios_base::in));
    }

    std::size_t size() const {
        return stream.view().size();
    }

};


TEST_CASE("glob::matcher: star_matcher", "[glob][glob::matcher]") {

    match_tester t("A тест");
    g::matcher_ptr m = std::make_unique<g::star_matcher>();

    SECTION("consume all when last") {
        REQUIRE( m->matches(t.buf, 0, true) );
        REQUIRE( t.pos() == t.size() );
    }
    
    SECTION("dont consume on first pass") {
        REQUIRE( m->matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 0 );
    }

    SECTION("must skip repetition utf8 codepoints") {
        REQUIRE( m->matches(t.buf, 3, false) );
        REQUIRE( t.pos() == 4 );
    }

    SECTION("dont match if there not enough octets") {
        REQUIRE_FALSE( m->matches(t.buf, t.size() + 1, false) );
        REQUIRE( t.pos() == t.size() );
    }
}

TEST_CASE("glob::matcher: codepoint_matcher", "[glob][glob::matcher]") {

    g::matcher_ptr m = std::make_unique<g::codepoint_matcher>(
        std::make_unique<g::or_predicate>(
            make_unique_container<g::predicate_vector>(
                std::make_unique<g::range_predicate>(U'А', U'Я'),
                std::make_unique<g::range_predicate>(U'a', U'z')
            )
        )
    );

    SECTION("must match utf8 codepoint") {
        match_tester t("ДА");
        REQUIRE( m->matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 2 );
    }

    SECTION("must match ansi codepoint") {
        match_tester t("yes");
        REQUIRE( m->matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 1 );
    }

    SECTION("dont match digit") {
        match_tester t("15");
        REQUIRE_FALSE( m->matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 1 );
    }

    SECTION("dont match empty") {
        match_tester t("");
        REQUIRE_FALSE( m->matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 0 );
    }

}

TEST_CASE("glob::matcher: string_matcher", "[glob][glob::matcher]") {

    g::string str = "тест";
    g::matcher_ptr m = std::make_unique<g::string_matcher>(str);

    SECTION("must match same string") {
        match_tester t("тест xyz");
        REQUIRE( m->matches(t.buf, 0, false) );
        REQUIRE( t.pos() == str.size() );
    }

    SECTION("must not match partial") {
        match_tester t("тес");
        REQUIRE_FALSE( m->matches(t.buf, 0, false) );
        REQUIRE( t.pos() == t.size() );
    }

    SECTION("must not match empty") {
        match_tester t("");
        REQUIRE_FALSE( m->matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 0 );
    }

}