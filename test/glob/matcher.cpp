#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <sstream>
#include <glob/glob.hpp>
#include <encoding/utf8.hpp>

#include "match_tester.hpp"

using namespace lf;

using g = glob<utf8_encoding>;

TEST_CASE("glob::matcher: star_matcher", "[glob::matcher]") {

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

TEST_CASE("glob::matcher: char_matcher test", "[glob::matcher]") {
    const bool negate = GENERATE(false, true);
    g::char_matcher p(negate, g::char_matcher::map_type {
        { U'0', U'0' },
        { U'4', U'4' },
        { U'5', U'5' },
        { U'A', U'Z' },
        { U'a', U'z' }
    });

    REQUIRE( p.test('.') == negate );
    REQUIRE( p.test('d') != negate );
    REQUIRE( p.test('E') != negate );
    REQUIRE( p.test('0') != negate );
    REQUIRE( p.test('1') == negate );
    REQUIRE( p.test('4') != negate );
    REQUIRE( p.test('5') != negate );
    REQUIRE( p.test('6') == negate );
    REQUIRE( p.test('~') == negate );
    REQUIRE( p.test('_') == negate );
}

TEST_CASE("glob::matcher: char_matcher add single", "[glob::matcher]") {
    g::char_matcher p;
    p.add(U'B');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'B'}});

    p.add(U'B');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'B'}});

    p.add(U'D');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'B'}, {U'D', U'D'}});

    p.add(U'C');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'D'}});

    p.add(U'E');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'E'}});

    p.add(U'A');
    REQUIRE(p.map == g::char_matcher::map_type {{U'A', U'E'}});
}

TEST_CASE("glob::matcher: char_matcher add range", "[glob::matcher]") {
    g::char_matcher p;
    p.add(U'B', U'D');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'D'}});

    p.add(U'B', U'D');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'D'}});

    p.add(U'F', U'H');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'D'}, {U'F', U'H'}});

    p.add(U'G', U'I');
    REQUIRE(p.map == g::char_matcher::map_type {{U'B', U'D'}, {U'F', U'I'}});

    p.add(U'A', U'B');
    REQUIRE(p.map == g::char_matcher::map_type {{U'A', U'D'}, {U'F', U'I'}});

    p.add(U'B', U'H');
    REQUIRE(p.map == g::char_matcher::map_type {{U'A', U'I'}});
}

TEST_CASE("glob::matcher: char_matcher", "[glob::matcher]") {

    g::char_matcher m(false, g::char_matcher::map_type {
        {U'А', U'Я'},
        {U'a', U'z'}
    });

    SECTION("must match utf8 codepoint") {
        match_tester t("ДА");
        REQUIRE( m.matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 2 );
    }

    SECTION("must match ansi codepoint") {
        match_tester t("yes");
        REQUIRE( m.matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 1 );
    }

    SECTION("dont match digit") {
        match_tester t("15");
        REQUIRE_FALSE( m.matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 1 );
    }

    SECTION("dont match empty") {
        match_tester t("");
        REQUIRE_FALSE( m.matches(t.buf, 0, false) );
        REQUIRE( t.pos() == 0 );
    }

}

TEST_CASE("glob::matcher: string_matcher", "[glob::matcher]") {

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