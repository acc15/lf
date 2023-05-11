#include <catch2/catch_test_macros.hpp>

#include "cmd/impl/del_cmd.hpp"
#include "test_util.hpp"

using namespace lf;
namespace fs = std::filesystem;

TEST_CASE("del_cmd: must remove path from index", "[del_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.local;
    
    auto index = lf::index {};
    index.set(test_path1, sync_mode::SHALLOW);
    index.set(test_path2, sync_mode::RECURSIVE);

    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { test_path1_str, test_path2_str } }
    }, index);

    REQUIRE( del_cmd().run(ctx) );
    REQUIRE( ctx.index->changed );
    REQUIRE( ctx.index->node(test_path1) == nullptr );
    REQUIRE( ctx.index->node(test_path2) == nullptr );
    REQUIRE_FALSE( ctx.state->changed );
}

TEST_CASE("del_cmd: must set IGNORE if soft delete specified", "[del_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.local;

    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { test_path1_str } },
        { "soft", {} }
    });

    REQUIRE( del_cmd().run(ctx) );
    REQUIRE(ctx.index->changed);
    REQUIRE(ctx.index->get(test_path1) == sync_mode::IGNORE);
}

TEST_CASE("del_cmd: must delete file if force specified", "[del_cmd]") {
    const auto cfg = make_test_config();
    cd_changer cd = cfg.local;

    write_text(test_path1, "test");
    
    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { test_path1_str } },
        { "force", {} }
    });

    del_cmd().run(ctx);
    REQUIRE_FALSE(fs::exists(test_path1));
}