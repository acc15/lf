#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <iostream>
#include <sstream>

#include "config/config.hpp"
#include "io/format.hpp"
#include "io/joiner.hpp"

#include "../io/log_tester.hpp"
#include "../test_util.hpp"

using namespace lf;

void cmp_sync(const config& cfg, const std::string& name, const config_sync& r) {
    INFO("sync=" << name);

    auto it = cfg.find(name);
    REQUIRE( it != cfg.end() );

    const config_sync& l = it->second;
    CHECK( l.local == r.local );
    CHECK( l.remote == r.remote );
    CHECK( l.index == r.index );
    CHECK( l.state == r.state );
}

config test_parse(const std::string& yaml) {
    std::stringstream ss(yaml);

    config cfg;
    ss >> with_ref_format<format::TEXT>(cfg);
    return cfg;
}

TEST_CASE("parse", "[config]") {

    const std::string yaml = 
        "[home]\n"
        "local=" + test_pstr("local/home") + "\n"
        "remote=" + test_pstr("remote/home") + "\n"
        "state=.config/lf/home.state\n"
        "index=home.index\n"
        "[pic]\n"
        "local=" + test_pstr("local/pic") + "\n"
        "remote=" + test_pstr("remote/pic") + "\n"
        "state=.config/lf/pic.state\n"
        "index=pic.index\n";

    auto p = test_parse(yaml);
    cmp_sync(p, "home", config_sync {
        .local = test_root / "local/home", 
        .remote = test_root / "remote/home",
        .state = test_root / "local/home/.config/lf/home.state", 
        .index = test_root / "remote/home/home.index"
    });
    cmp_sync(p, "pic", config_sync {
        .local = test_root / "local/pic", 
        .remote = test_root / "remote/pic",
        .state = test_root / "local/pic/.config/lf/pic.state", 
        .index = test_root / "remote/pic/pic.index"
    });

}

TEST_CASE("empty section", "[config]") {
    log_tester t;
    test_parse("state=test.abc");
    REQUIRE_THAT(t.str(), Catch::Matchers::ContainsSubstring("empty section"));
}

TEST_CASE("parse partial", "[config]") {
    log_tester t;
    auto p = test_parse(
        "[home]\n"
        "index=test.index\n"
    );
    REQUIRE( p.empty() );
    
    const auto msg = t.str();
    REQUIRE(msg.find("remote") != std::string::npos);
    REQUIRE(msg.find("local") != std::string::npos);
    REQUIRE(msg.find("state") != std::string::npos);
}


