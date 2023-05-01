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

void cmp_sync(const config& cfg, const config::sync& r) {
    INFO("sync=" << r.name);

    auto it = cfg.find_by_name(r.name);
    REQUIRE( it != cfg.syncs.end() );

    const config::sync& l = *it;
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
        "[home]\n"
        "local=" + test_root_str("local/home") + "\n"
        "remote=" + test_root_str("remote/home") + "\n"
        "state=.config/lf/home.state\n"
        "index=home.index\n"
        "[pic]\n"
        "local=" + test_root_str("local/pic") + "\n"
        "remote=" + test_root_str("remote/pic") + "\n"
        "state=.config/lf/pic.state\n"
        "index=pic.index\n";

    auto p = test_parse(text);
    cmp_sync(p, config::sync {
        .name = "home",
        .local = test_root_path / "local/home", 
        .remote = test_root_path / "remote/home",
        .state = test_root_path / "local/home/.config/lf/home.state", 
        .index = test_root_path / "remote/home/home.index"
    });
    cmp_sync(p, config::sync {
        .name = "pic",
        .local = test_root_path / "local/pic", 
        .remote = test_root_path / "remote/pic",
        .state = test_root_path / "local/pic/.config/lf/pic.state", 
        .index = test_root_path / "remote/pic/pic.index"
    });

}

TEST_CASE("config: empty section", "[config]") {
    log_tester t(INFO);
    test_parse("state=test.abc");
    REQUIRE_THAT(t.str(), Catch::Matchers::ContainsSubstring("empty section"));
}

TEST_CASE("config: parse partial", "[config]") {
    log_tester t(INFO);
    auto p = test_parse(
        "[home]\n"
        "index=test.index\n"
    );
    REQUIRE( p.syncs.empty() );
    
    const auto msg = t.str();
    REQUIRE(msg.find("remote") != std::string::npos);
    REQUIRE(msg.find("local") != std::string::npos);
}

TEST_CASE("config: find_local_matches", "[config]") {
    auto home_path = test_root_path / "home";
    config cfg = {
        .syncs = {
            { .name = "home", .local = home_path, .remote {}, .state {}, .index {} },
            { .name = "pic", .local = home_path / "pic", .remote {}, .state {}, .index {} }
        }
    };

    auto test_path = home_path / "pic" / "test.jpg";
    
    config::sync_entry_vec found_syncs = cfg.find_most_specific_local_matches(test_path);
    REQUIRE( found_syncs.size() == 1 );
    REQUIRE( found_syncs[0]->name == "pic" );
    
}

