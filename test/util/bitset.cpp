#include <catch2/catch.hpp>

#include <util/bitset.hpp>

using leafsync::bitset;

TEST_CASE( "default_constructor", "[bitset]" ) {
    bitset b;
    REQUIRE( b.blocks().empty() );
    REQUIRE( b.size() == 0 );
}

TEST_CASE( "copy_constructor", "[bitset]" ) {
    bitset b1;
    b1.resize(5);
    b1.set(2, true);

    bitset b2(b1);
    REQUIRE( b1.size() == 5 );
    REQUIRE( b2.size() == 5 );
    REQUIRE( b2.blocks() == b1.blocks() );
}

TEST_CASE( "move_constructor", "[bitset]" ) {
    bitset b1;
    b1.resize(5);
    b1.set(2, true);

    bitset b2(std::move(b1));
    REQUIRE( b1.size() == 0 );
    REQUIRE( b2.size() == 5 );
    REQUIRE( b1.blocks().empty() );
    REQUIRE( b2.blocks().size() == 1 );
}

TEST_CASE( "copy_assign", "[bitset]" ) {
    bitset b1;
    b1.resize(5);
    b1.set(2, true);

    bitset b2;
    b2 = b1;

    REQUIRE( b1.size() == 5 );
    REQUIRE( b2.size() == 5 );
    REQUIRE( b1.blocks().size() == 1 );
    REQUIRE( b2.blocks().size() == 1 );
}

TEST_CASE( "move_assign", "[bitset]" ) {
    bitset b1;
    b1.resize(5);
    b1.set(2, true);

    bitset b2;
    b2.resize(10);
    b2 = std::move(b1);

    REQUIRE( b1.size() == 0 );
    REQUIRE( b2.size() == 5 );
    REQUIRE( b1.blocks().empty() );
    REQUIRE( !b2.blocks().empty() );
}

TEST_CASE( "initial_values_is_false", "[bitset]" ) {
    bitset b;
    b.resize(4);
    REQUIRE(b.size() == 4);
    for (bitset::size_type i = 0; i < b.size(); i++) {
        REQUIRE( b.get(i) == false );
    }
}

TEST_CASE( "set_get", "[bitset]" ) {
    bitset b;
    b.resize(4);
    b.set(0, false);
    b.set(1, true);
    b.set(2, false);
    b.set(3, true);

    REQUIRE( b.get(0) == false );
    REQUIRE( b.get(1) == true );
    REQUIRE( b.get(2) == false );
    REQUIRE( b.get(3) == true );
}

TEST_CASE( "resize_must_initialize_buf", "[bitset]" ) {
    bitset b;
    b.resize(10);
    REQUIRE( !b.blocks().empty() );
    REQUIRE( b.size() == 10 );
    REQUIRE( b.blocks().size() == 2 );

    REQUIRE( b.blocks()[0] == 0 );
    REQUIRE( b.blocks()[1] == 0 );
}

TEST_CASE( "resize_should_keep_values", "[bitset]" ) {
    bitset b;
    b.resize(5);
    b.set(2, true);
    b.set(3, true);
    b.set(4, false);

    b.resize(10);

    REQUIRE( b.get(2) == true );
    REQUIRE( b.get(3) == true );
    REQUIRE( b.get(4) == false );
}