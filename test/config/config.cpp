#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <sstream>
#include <utility>

#include "config/config.hpp"
#include "io/errors.hpp"
#include "io/with_format.hpp"
#include "../test_util.hpp"

#include "io/joiner.hpp"

using namespace lf;

void cmp_sync(const config& cfg, const std::string& name, const config_sync& r) {
    INFO("sync=" << name);

    auto it = cfg.find(name);
    REQUIRE( it != cfg.end() );

    const config_sync& l = it->second;
    CHECK( l.index == r.index );
    CHECK( l.local == r.local );
    CHECK( l.remote == r.remote );
}

std::pair<config, errors> test_parse(const std::string& yaml) {
    std::stringstream ss(yaml);

    config cfg;
    errors errors("test");
    REQUIRE( ss >> with_ref_format<format::YAML>(cfg, errors) );
    return std::make_pair(cfg, errors);
}

TEST_CASE("parse", "[config]") {

    const std::string yaml = 
        "home: \n"
        "  local: " + test_pstr("local/home") + "\n"
        "  remote: " + test_pstr("remote/home") + "\n"
        "  index: .config/lf/home.index\n"
        "pic: \n"
        "  local: " + test_pstr("local/pic") + "\n"
        "  remote: " + test_pstr("remote/pic") + "\n"
        "  index: .config/lf/pic.index\n";

    auto p = test_parse(yaml);
    REQUIRE( !p.second.has_errors() );
    cmp_sync(p.first, "home", config_sync {
        .index = test_root / "local/home/.config/lf/home.index", 
        .local = test_root / "local/home", 
        .remote = test_root / "remote/home"
    });
    cmp_sync(p.first, "pic", config_sync {
        .index = test_root / "local/pic/.config/lf/pic.index", 
        .local = test_root / "local/pic", 
        .remote = test_root / "remote/pic"
    });

}

TEST_CASE("parse array", "[config]") {
    auto p = test_parse("[1,2,3]");
    REQUIRE(p.second.has_errors());
}

TEST_CASE("parse without required fields", "[config]") {
    auto p = test_parse(
        "home: \n"
        "  test: abc"
    );
    REQUIRE(p.second.has_errors());
}
