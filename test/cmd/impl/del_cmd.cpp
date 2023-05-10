#include <catch2/catch_test_macros.hpp>

#include "cmd/impl/del_cmd.hpp"
#include "test_util.hpp"

using namespace lf;
namespace fs = std::filesystem;

const auto test_path = fs::path("a") / "b" / "test.txt";
const auto test_path2 = fs::path("x") / "y" / "z";

TEST_CASE("del_cmd: must remove path from index", "[del_cmd]") {
    const auto cfg = make_test_config();

    const auto p1 = (cfg.local / test_path).string();
    const auto p2 = (cfg.local / test_path2).string();
    
    auto index = lf::index {};
    index.set(test_path, sync_mode::SHALLOW);
    index.set(test_path2, sync_mode::RECURSIVE);

    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { p1, p2 } }
    }, index);

    del_cmd().run(ctx);

    REQUIRE( ctx.index->changed );
    REQUIRE( ctx.index->node(test_path) == nullptr );
    REQUIRE( ctx.index->node(test_path2) == nullptr );
    REQUIRE_FALSE( ctx.state->changed );
}

TEST_CASE("del_cmd: must set IGNORE if soft delete specified", "[del_cmd]") {
    const auto cfg = make_test_config();
    const auto p1 = (cfg.local / test_path).string();

    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { p1 } },
        { "soft", {} }
    });

    del_cmd().run(ctx);

    REQUIRE(ctx.index->changed);
    REQUIRE(ctx.index->get(test_path) == sync_mode::IGNORE);
}

TEST_CASE("del_cmd: must delete file if force specified", "[del_cmd]") {
    const auto cfg = make_test_config();
    const auto abs_path = cfg.local / test_path;
    
    write_text(abs_path, "test");
    
    const auto p1 = abs_path.string();
    cmd_context ctx = make_test_cmd_context(cfg, {
        { "", { p1 } },
        { "force", {} }
    });

    del_cmd().run(ctx);
    REQUIRE_FALSE(fs::exists(abs_path));
}