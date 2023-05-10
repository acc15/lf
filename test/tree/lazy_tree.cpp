#include <catch2/catch_test_macros.hpp>

#include "tree/lazy_tree.hpp"
#include "index/index.hpp"
#include "test_util.hpp"

using namespace lf;

namespace fs = std::filesystem;

TEST_CASE("lazy_tree: must not open file and use model when init provided", "[lazy_tree]") {
    lazy_tree<lf::index> index("a.index", lf::index { sync_mode::RECURSIVE });
    REQUIRE( index.get_or_load().root.data == sync_mode::RECURSIVE );
    REQUIRE_FALSE( index.save_if_changed() );
}

TEST_CASE("lazy_tree: must load index from file", "[lazy_tree]") {

    const fs::path test_dir = create_temp_test_dir();
    const fs::path index_file = test_dir / "test.index";

    lf::index expect;

    const fs::path shallow_file = fs::path("a") / "b" / "c.txt";
    const fs::path recursive_dir = fs::path("x") / "y" / "z";
    const fs::path ignore_dir = recursive_dir / "w";

    expect.set(shallow_file, sync_mode::SHALLOW);
    expect.set(recursive_dir, sync_mode::RECURSIVE);
    expect.set(ignore_dir, sync_mode::IGNORE);
    save_file<tree_binary_format>(index_file, expect);

    lazy_tree<lf::index> load_index(index_file);

    lf::index& actual = load_index.get_or_load().root;
    REQUIRE( actual.data == expect.data );
    REQUIRE( actual.get(shallow_file) == expect.get(shallow_file) );
    REQUIRE( actual.get(recursive_dir) == expect.get(recursive_dir) );
    REQUIRE( actual.get(ignore_dir) == expect.get(ignore_dir) );

}

TEST_CASE("lazy_tree: must save index on change", "[lazy_tree]") {

    const fs::path test_dir = create_temp_test_dir();
    const fs::path index_file = test_dir / "test.index";

    const fs::path recursive_dir = fs::path("a") / "b" / "c";

    lazy_tree<lf::index> load_index(index_file);
    load_index.get_or_load().set(recursive_dir, sync_mode::RECURSIVE);
    REQUIRE( load_index.close() );

    lf::index actual;
    load_file<tree_binary_format>(index_file, actual);
    REQUIRE( actual.get(recursive_dir) == sync_mode::RECURSIVE );

}