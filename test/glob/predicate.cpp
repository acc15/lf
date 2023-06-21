#include <catch2/catch_test_macros.hpp>

#include <glob/glob.hpp>
#include <encoding/utf8.hpp>
#include "../test_util.hpp"

using namespace lf;

using g = glob<utf8_encoding>;

TEST_CASE("glob::predicate: set", "[glob][glob::predicate]") {
    auto p = std::make_unique<g::set_predicate>(std::unordered_set<g::codepoint> { 'a', 'b', 'd' });
    REQUIRE( p->test('a') );
    REQUIRE( p->test('b') );
    REQUIRE_FALSE( p->test('c') );
    REQUIRE( p->test('d') );
}

TEST_CASE("glob::predicate: range", "[glob][glob::predicate]") {
    auto p = std::make_unique<g::range_predicate>('a', 'z');
    REQUIRE( p->test('b') );
    REQUIRE( p->test('a') );
    REQUIRE_FALSE( p->test('0') );
}

TEST_CASE("glob::predicate: not", "[glob][glob::predicate]") {
    auto p = std::make_unique<g::not_predicate>(std::make_unique<g::range_predicate>('a', 'z'));
    REQUIRE_FALSE( p->test('b') );
    REQUIRE_FALSE( p->test('a') );
    REQUIRE( p->test('0') );
}

TEST_CASE("glob::predicate: any", "[glob][glob::predicate]") {
    auto p = std::make_unique<g::any_predicate>();
    REQUIRE( p->test('b') );
    REQUIRE( p->test('a') );
    REQUIRE( p->test('0') );
}

TEST_CASE("glob::predicate: or", "[glob][glob::predicate]") {
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

TEST_CASE("glob::predicate: class", "[glob][glob::predicate]") {
    g::predicate_ptr p = std::make_unique<g::class_predicate>(std::map<g::codepoint, g::codepoint> {
        { U'0', U'0' },
        { U'4', U'4' },
        { U'5', U'5' },
        { U'A', U'Z' },
        { U'a', U'z' }
    });
    REQUIRE_FALSE( p->test('.') );
    REQUIRE( p->test('d') );
    REQUIRE( p->test('E') );
    REQUIRE( p->test('0') );
    REQUIRE_FALSE( p->test('1') );
    REQUIRE( p->test('4') );
    REQUIRE( p->test('5') );
    REQUIRE_FALSE( p->test('6') );
    REQUIRE_FALSE( p->test('~') );
    REQUIRE_FALSE( p->test('_') );
}

TEST_CASE("glob::predicate: class add single", "[glob][glob::predicate]") {
    g::class_predicate p;
    p.add(U'B');
    REQUIRE(p.map == g::class_predicate::map_type {{U'B', U'B'}});

    p.add(U'D');
    REQUIRE(p.map == g::class_predicate::map_type {{U'B', U'B'}, {U'D', U'D'}});

    p.add(U'C');
    REQUIRE(p.map == g::class_predicate::map_type {{U'B', U'D'}});

    p.add(U'E');
    REQUIRE(p.map == g::class_predicate::map_type {{U'B', U'E'}});

    p.add(U'A');
    REQUIRE(p.map == g::class_predicate::map_type {{U'A', U'E'}});
}

TEST_CASE("glob::predicate: class add range", "[glob][glob::predicate]") {
    g::class_predicate p;
    p.add(U'B', U'D');
    REQUIRE(p.map == g::class_predicate::map_type {{U'B', U'D'}});

    p.add(U'F', U'H');
    REQUIRE(p.map == g::class_predicate::map_type {{U'B', U'D'}, {U'F', U'H'}});

    p.add(U'G', U'I');
    REQUIRE(p.map == g::class_predicate::map_type {{U'B', U'D'}, {U'F', U'I'}});

    p.add(U'A', U'B');
    REQUIRE(p.map == g::class_predicate::map_type {{U'A', U'D'}, {U'F', U'I'}});

    p.add(U'B', U'H');
    REQUIRE(p.map == g::class_predicate::map_type {{U'A', U'I'}});

}
