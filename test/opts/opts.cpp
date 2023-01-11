#include <catch2/catch_test_macros.hpp>

#include "opts/opts.hpp"

using namespace lf;

const opts test_opts = {
    {
        opt { "dry", 'd', "dry mode, just print actions" },
        opt { "shallow", 'S', "add paths with SHALLOW mode", "path", 0, opt::UNBOUNDED },
        opt { "recursive", 'R', "add paths with RECURSIVE mode", "dirpath", 0, opt::UNBOUNDED },
        opt { "ignore", 'I', "add paths with IGNORE mode", "path", 0, opt::UNBOUNDED },
        opt { "unused", '\0', "unused opt" }
    },
    "shallow"
};

TEST_CASE("parse", "[opts]") {

    const char* argv[] = { "-R", "a", "\\escape", "/root", "-I", "-d", "default", "shallow" };
    const opt_map m = test_opts.parse(argv);
    REQUIRE( m["recursive"] == std::vector<std::string_view> { "a", "escape", "/root" } );
    REQUIRE( m["shallow"] == std::vector<std::string_view> { "default", "shallow" } );
    REQUIRE( m.has("ignore") );
    REQUIRE( m["ignore"].empty() );
    REQUIRE( m.has("dry") );
    REQUIRE_FALSE( m.has("unused") );
    
}

TEST_CASE("usage", "[opts]") {
    std::stringstream s;
    s << test_opts;
    REQUIRE( s.str() == 
        "    -d, --dry - dry mode, just print actions\n" 
        "    -S, --shallow path - add paths with SHALLOW mode\n" 
        "    -R, --recursive dirpath - add paths with RECURSIVE mode\n" 
        "    -I, --ignore path - add paths with IGNORE mode\n" 
        "    --unused - unused opt\n" 
    );
}