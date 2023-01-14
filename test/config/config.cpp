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

void cmp_sync(const config& cfg, const std::string& name, const config::sync& r) {
    INFO("sync=" << name);

    auto it = cfg.syncs.find(name);
    REQUIRE( it != cfg.syncs.end() );

    const config::sync& l = it->second;
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
        "local=" + test_root_str("local/home") + "\n"
        "remote=" + test_root_str("remote/home") + "\n"
        "state=.config/lf/home.state\n"
        "index=home.index\n"
        "[pic]\n"
        "local=" + test_root_str("local/pic") + "\n"
        "remote=" + test_root_str("remote/pic") + "\n"
        "state=.config/lf/pic.state\n"
        "index=pic.index\n";

    auto p = test_parse(yaml);
    cmp_sync(p, "home", config::sync {
        .local = test_root_path / "local/home", 
        .remote = test_root_path / "remote/home",
        .state = test_root_path / "local/home/.config/lf/home.state", 
        .index = test_root_path / "remote/home/home.index"
    });
    cmp_sync(p, "pic", config::sync {
        .local = test_root_path / "local/pic", 
        .remote = test_root_path / "remote/pic",
        .state = test_root_path / "local/pic/.config/lf/pic.state", 
        .index = test_root_path / "remote/pic/pic.index"
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
    REQUIRE( p.syncs.empty() );
    
    const auto msg = t.str();
    REQUIRE(msg.find("remote") != std::string::npos);
    REQUIRE(msg.find("local") != std::string::npos);
}

TEST_CASE("find_local_matches", "[config]") {
    auto home_path = test_root_path / "home";
    config cfg = {
        .syncs = {
            { "home", config::sync { .local = home_path, .remote {}, .state {}, .index {} } },
            { "pic", config::sync { .local = home_path / "pic", .remote {}, .state {}, .index {} } }
        }
    };

    auto test_path = home_path / "pic" / "test.jpg";
    
    config::sync_entry_vec found_syncs = cfg.find_most_specific_local_matches(test_path);
    REQUIRE( found_syncs.size() == 1 );
    REQUIRE( found_syncs[0]->first == "pic" );
    
}

