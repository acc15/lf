#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <iostream>
#include <sstream>

#include "config/config.hpp"
#include "io/format.hpp"
#include "io/joiner.hpp"

#include "log/log_tester.hpp"
#include "test_util.hpp"

using namespace lf;

void cmp_config(const config& l, const config& r) {
    CHECK( l.left == r.left );
    CHECK( l.right == r.right );
    CHECK( l.index == r.index );
    CHECK( l.state == r.state );
}

config test_parse(const std::string& yaml) {
    std::stringstream ss(yaml);
    return config::parse(ss);
}

TEST_CASE("config: parse", "[config]") {

    const std::string text = 
        "left=" + test_root_str("left/home") + "\n"
        "right=" + test_root_str("right/home") + "\n"
        "state=.config/lf/home.state\n"
        "index=home.index\n";

    auto p = test_parse(text);
    cmp_config(p, config {
        .left = test_root_path / "left/home", 
        .right = test_root_path / "right/home",
        .state = test_root_path / "left/home/.config/lf/home.state", 
        .index = test_root_path / "right/home/home.index"
    });

}

TEST_CASE("config: parse invalid", "[config]") {
    REQUIRE_THROWS( test_parse("index=test.index\n") );
}

