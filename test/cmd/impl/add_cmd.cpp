#include <catch2/catch_test_macros.hpp>

#include "cmd/impl/add_cmd.hpp"
#include "test_util.hpp"

using namespace lf;
namespace fs = std::filesystem;

const auto test_path = fs::path("a") / "b" / "test.txt";
const auto test_path2 = fs::path("x") / "y" / "z";

TEST_CASE("must add paths to index", "[add_cmd]") {
    const auto cfg = make_test_config();

    const auto p1 = (cfg.local / test_path).string();
    const auto p2 = (cfg.local / test_path2).string();
    
    cmd_context ctx = make_test_cmd_context(cfg, {
        { "shallow", { p1 } },
        { "recursive", { p2 } }
    });

    add_cmd().run(ctx);

    REQUIRE( ctx.index->changed );
    REQUIRE( ctx.index->get(test_path) == sync_mode::SHALLOW );
    REQUIRE( ctx.index->get(test_path2) == sync_mode::RECURSIVE );
    REQUIRE_FALSE( ctx.state->changed );
}
