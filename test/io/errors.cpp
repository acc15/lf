#include <catch2/catch_test_macros.hpp>

#include "../../src/io/errors.hpp"
#include "../test_util.hpp"

using namespace lf;

TEST_CASE("error formatting", "[errors]") {

    test_errors err;
    err << "test message" << errors::end;

    REQUIRE(err.messages.size() == 1);
    REQUIRE(err.messages[0] == "test message");

}