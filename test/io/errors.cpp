#include <catch2/catch_test_macros.hpp>

#include "io/errors.hpp"
#include "log_tester.hpp"

using namespace lf;

TEST_CASE("error formatting", "[errors]") {
    log_tester t(log::INFO);

    errors err("test");
    err << "test message" << errors::end;

    auto lines = t.lines();
    REQUIRE(lines.size() == 3);
    REQUIRE(lines[2] == " - test message");

}