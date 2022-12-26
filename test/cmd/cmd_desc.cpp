#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "cmd/cmd_desc.hpp"

using namespace lf;

TEST_CASE("print", "[cmd_desc]") {
    const char* names[] = {"a", "b", "c"};
    cmd_desc desc = {names, "params", "description"};

    std::stringstream ss;
    ss << desc;

    REQUIRE(ss.str() == "(a|b|c) params - description\n");
}
