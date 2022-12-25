#include <catch2/catch_test_macros.hpp>

#include <cmd/cmd_desc.hpp>
#include <sstream>

using namespace lf;

TEST_CASE("print", "[cmd_desc]") {
    const char* names[] = {"a", "b", "c"};
    cmd_desc desc = {.names = names, .description = "test"};

    std::stringstream ss;
    ss << desc;

    REQUIRE(ss.str() == "a, b, c - test");
}
