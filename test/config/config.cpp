#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <iostream>
#include <sstream>

#include "config/config.hpp"
#include "io/format.hpp"
#include "io/joiner.hpp"

#include "io/log_tester.hpp"
#include "test_util.hpp"

using namespace lf;

void cmp_config(const config& l, const config& r) {
    CHECK( l.local == r.local );
    CHECK( l.remote == r.remote );
    CHECK( l.index == r.index );
    CHECK( l.state == r.state );
}

config test_parse(const std::string& yaml) {
    std::stringstream ss(yaml);

    config cfg;
    ss >> read_as<config_format>(cfg);
    return cfg;
}

TEST_CASE("config: parse", "[config]") {

    const std::string text = 
        "local=" + test_root_str("local/home") + "\n"
        "remote=" + test_root_str("remote/home") + "\n"
        "state=.config/lf/home.state\n"
        "index=home.index\n";

    auto p = test_parse(text);
    cmp_config(p, config {
        .local = test_root_path / "local/home", 
        .remote = test_root_path / "remote/home",
        .state = test_root_path / "local/home/.config/lf/home.state", 
        .index = test_root_path / "remote/home/home.index"
    });

}

TEST_CASE("config: parse partial", "[config]") {
    log_tester t(INFO);
    auto p = test_parse("index=test.index\n");
    
    const auto msg = t.str();
    REQUIRE(msg.find("remote") != std::string::npos);
    REQUIRE(msg.find("local") != std::string::npos);
}

