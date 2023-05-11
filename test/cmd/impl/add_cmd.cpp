#include <catch2/catch_test_macros.hpp>

#include "cmd/impl/add_cmd.hpp"
#include "test_util.hpp"

using namespace lf;

namespace fs = std::filesystem;

TEST_CASE("add_cmd: must skip non-existent paths to index", "[add_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.local;

    cmd_context ctx = make_test_cmd_context(cfg, {
        { "shallow", { test_path1_str } },
        { "recursive", { test_path2_str } }
    });
    REQUIRE_FALSE( add_cmd().run(ctx) );
    REQUIRE_FALSE( ctx.index->changed );
    REQUIRE_FALSE( ctx.state->changed );
}

TEST_CASE("add_cmd: must add non-existent file to index when index flag set", "[add_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.local;

    cmd_context ctx = make_test_cmd_context(cfg, { { "shallow", { test_path1_str } }, { "index", {} } });

    REQUIRE( add_cmd().run(ctx) );
    REQUIRE( ctx.index->changed );
    REQUIRE( ctx.index->get(test_path1) == sync_mode::SHALLOW );
    REQUIRE_FALSE( ctx.state->changed );
}

TEST_CASE("add_cmd: must add existent file", "[add_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.local;

    write_text(test_path1, "abc");
    cmd_context ctx = make_test_cmd_context(cfg, { { "shallow", { test_path1_str } } });

    REQUIRE( add_cmd().run(ctx) );
    REQUIRE( ctx.index->changed );
    REQUIRE( ctx.index->get(test_path1) == sync_mode::SHALLOW );
    REQUIRE_FALSE( ctx.state->changed );
}

TEST_CASE("add_cmd: must avoid adding file with recursive mode", "[add_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.local;

    write_text(test_path1, "abc");
    cmd_context ctx = make_test_cmd_context(cfg, { { "recursive", { test_path1_str } } });

    REQUIRE_FALSE( add_cmd().run(ctx) );
    REQUIRE_FALSE( ctx.index->changed );
    REQUIRE_FALSE( ctx.state->changed );
}

TEST_CASE("add_cmd: must add dir with recursive mode", "[add_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.local;

    fs::create_directories(test_path1);    
    cmd_context ctx = make_test_cmd_context(cfg, { { "recursive", { test_path1_str } } });

    REQUIRE( add_cmd().run(ctx) );
    REQUIRE( ctx.index->changed );
    REQUIRE( ctx.index->get(test_path1) == sync_mode::RECURSIVE );
    REQUIRE_FALSE( ctx.state->changed );
}