#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <vector>
#include <sstream>

#include "config/config_parser.hpp"
#include "io/log_tester.hpp"

struct config_item {
    std::size_t line;
    std::string section;
    std::string key;
    std::string value;
};

using config_vec = std::vector<config_item>;

void cmp_parse_config_vec(const config_vec& l, const config_vec& r) {
    REQUIRE( l.size() == r.size() );
    for (size_t i = 0; i < l.size(); i++) {
        INFO("at " << i << " item");
        CHECK( l[i].line == r[i].line );
        CHECK( l[i].section == r[i].section );
        CHECK( l[i].key == r[i].key );
        CHECK( l[i].value == r[i].value );
    } 
}

config_vec test_parse_config(const std::string& text) {
    std::stringstream ss(text);
    config_vec result;
    lf::parse_config(ss, [&result](const lf::config_entry& e) {
        result.emplace_back(config_item {
            .line = e.line,
            .section = e.section,
            .key = static_cast<std::string>(e.key),
            .value = static_cast<std::string>(e.value)
        });
    });
    REQUIRE(ss.eof());
    REQUIRE_FALSE(ss.fail());
    REQUIRE_FALSE(ss.bad());
    return result;
}

TEST_CASE("config_parser: parse", "[config_parser]") {
    config_vec actual = test_parse_config(
        "no_section = yes\n"
        " # comment \n"
        "\n"
        "[test]\n"
        "  a = b  \n"
        "b=c\n"
        " c  =   \n"
        "  [other]\n"
        " a = c\n"
        "[]\n"
        "again_no_section  =yes\n"
        ""
    );
    config_vec expected = {
        config_item { .line = 1, .section = "", .key = "no_section", .value = "yes" },
        config_item { .line = 5, .section = "test", .key = "a", .value = "b" },
        config_item { .line = 6, .section = "test", .key = "b", .value = "c" },
        config_item { .line = 7, .section = "test", .key = "c", .value = "" },
        config_item { .line = 9, .section = "other", .key = "a", .value = "c" },
        config_item { .line = 11, .section = "", .key = "again_no_section", .value = "yes" }
    };
    cmp_parse_config_vec(actual, expected);
}

TEST_CASE("config_parser empty key", "[config_parser]") {
    lf::log_tester t(lf::INFO);
    test_parse_config("=");
    REQUIRE_THAT(t.str(), Catch::Matchers::ContainsSubstring("empty key"));
}

