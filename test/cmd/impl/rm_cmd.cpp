#include <catch2/catch_test_macros.hpp>

#include "cmd/impl/rm_cmd.hpp"
#include "test_util.hpp"

using namespace lf;
namespace fs = std::filesystem;

TEST_CASE("rm_cmd: must remove path from index", "[rm_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.left;
    
    auto index = lf::index {};
    index.set(test_path1, sync_mode::SHALLOW);
    index.set(test_path2, sync_mode::RECURSIVE);

    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { test_path1_str, test_path2_str } }
    }, index);

    REQUIRE( rm_cmd().run(ctx) );
    REQUIRE( ctx.index->changed );
    REQUIRE( ctx.index->node(test_path1) == nullptr );
    REQUIRE( ctx.index->node(test_path2) == nullptr );
    REQUIRE_FALSE( ctx.state->changed );
}

TEST_CASE("rm_cmd: must set IGNORE if soft delete specified", "[rm_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.left;

    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { test_path1_str } },
        { "soft", {} }
    });

    REQUIRE( rm_cmd().run(ctx) );
    REQUIRE(ctx.index->changed);
    REQUIRE(ctx.index->get(test_path1) == sync_mode::IGNORE);
}

TEST_CASE("rm_cmd: must delete file if force specified", "[rm_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.left;

    write_text(test_path1, "test");
    
    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { test_path1_str } },
        { "force", {} }
    });

    rm_cmd().run(ctx);
    REQUIRE_FALSE(fs::exists(test_path1));
}