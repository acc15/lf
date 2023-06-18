#include <catch2/catch_test_macros.hpp>

#include <unicode/unicode.hpp>

#include <glob/glob.hpp>
#include <util/container.hpp>

using g = lf::glob<lf::utf8_encoding>;

TEST_CASE("dynamic_predicate: set", "[dynamic_predicate]") {
    auto p = std::make_unique<g::set_predicate>(std::unordered_set<g::codepoint> { 'a', 'b', 'd' });
    REQUIRE( p->test('a') );
    REQUIRE( p->test('b') );
    REQUIRE_FALSE( p->test('c') );
    REQUIRE( p->test('d') );
}

TEST_CASE("dynamic_predicate: range", "[glob][dynamic_predicate]") {
    auto p = std::make_unique<g::range_predicate>('a', 'z');
    REQUIRE( p->test('b') );
    REQUIRE( p->test('a') );
    REQUIRE_FALSE( p->test('0') );
}

TEST_CASE("dynamic_predicate: not", "[glob][dynamic_predicate]") {
    auto p = std::make_unique<g::not_predicate>(std::make_unique<g::range_predicate>('a', 'z'));
    REQUIRE_FALSE( p->test('b') );
    REQUIRE_FALSE( p->test('a') );
    REQUIRE( p->test('0') );
}

TEST_CASE("dynamic_predicate: any", "[glob][dynamic_predicate]") {
    auto p = std::make_unique<g::any_predicate>();
    REQUIRE( p->test('b') );
    REQUIRE( p->test('a') );
    REQUIRE( p->test('0') );
}

TEST_CASE("dynamic_predicate: or", "[glob][dynamic_predicate]") {
    auto p = std::make_unique<g::or_predicate>(make_unique_container<g::predicate_vector>(
        std::make_unique<g::range_predicate>('a', 'z'),
        std::make_unique<g::range_predicate>('A', 'Z'),
        std::make_unique<g::set_predicate>(std::unordered_set<g::codepoint> { '0', '4', '5' })
    ));
    REQUIRE( p->test('d') );
    REQUIRE( p->test('E') );
    REQUIRE( p->test('0') );
    REQUIRE_FALSE( p->test('1') );
    REQUIRE( p->test('4') );
    REQUIRE( p->test('5') );
    REQUIRE_FALSE( p->test('6') );
}