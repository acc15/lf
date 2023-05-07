#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <fstream>
#include <map>

#include "state/state.hpp"
#include "index/index.hpp"

#include "tree/print.hpp"
#include "tree/binary.hpp"

#include "io/log_tester.hpp"

using namespace lf;
using namespace std::filesystem;

const state test_tree = { false, {
    { "a", state { false, {
        { "01.json", state { false } }, 
        { "02.yaml", state { false } }
    }}},
    { "b", state { false, {
        { "03.sql", state { true } }, 
        { "04.jpg", state { false } }
    }}},
    { "c", state { false, {
        { "05.cpp", state { false } }, 
        { "06.hpp", state { true } }
    }}},
    { "d", state { false, {
        { "07.txt", state { true } }, 
        { "08.bin", state { true } }
    }}},
    { "e", state { true, {
        { "09.json", state { false } }, 
        { "10.yaml", state { false } }
    }}},
    { "f", state { true, {
        { "11.sql", state { true } }, 
        { "12.jpg", state { false } }
    }}},
    { "g", state { true, {
        { "13.cpp", state { false } }, 
        { "14.hpp", state { true } }
    }}},
    { "h", state { true, {
        { "15.txt", state { true } }, 
        { "16.bin", state { true } }
    }}}
}};

const path test_index_path = "b/03.sql";

TEST_CASE("tree: print", "[tree]") {
    std::stringstream ss;

    ss << std::boolalpha;
    tree_print<state>::print(ss, test_tree);
    
    const std::string str = ss.str();
    REQUIRE(str == 
        "<root> [ ]\n"
        "├── a [ ]\n"
        "│   ├── 01.json [ ]\n"
        "│   └── 02.yaml [ ]\n"
        "├── b [ ]\n"
        "│   ├── 03.sql [+]\n"
        "│   └── 04.jpg [ ]\n"
        "├── c [ ]\n"
        "│   ├── 05.cpp [ ]\n"
        "│   └── 06.hpp [+]\n"
        "├── d [ ]\n"
        "│   ├── 07.txt [+]\n"
        "│   └── 08.bin [+]\n"
        "├── e [+]\n"
        "│   ├── 09.json [ ]\n"
        "│   └── 10.yaml [ ]\n"
        "├── f [+]\n"
        "│   ├── 11.sql [+]\n"
        "│   └── 12.jpg [ ]\n"
        "├── g [+]\n"
        "│   ├── 13.cpp [ ]\n"
        "│   └── 14.hpp [+]\n"
        "└── h [+]\n"
        "    ├── 15.txt [+]\n"
        "    └── 16.bin [+]\n"
    );
}

template <tree_data_type T>
void cmp_tree(const tree<T>& l, const tree<T>& r) {
    {
        INFO("l=" << l.data << ",r=" << r.data);
        CHECK(l.data == r.data);
    }
    for (const auto& le: l.entries) {
        const auto rei = r.entries.find(le.first);
        INFO("node=" << le.first);
        if (rei == r.entries.end()) {
            FAIL_CHECK("missing node in r tree");
            continue;
        }
        cmp_tree(le.second, rei->second);
    }
    for (const auto& re: r.entries) {
        INFO("node=" << re.first);
        if (!l.entries.contains(re.first)) {
            FAIL_CHECK("missing node in r tree");
        }
    }
}

TEST_CASE("tree: get", "[tree]") {
    REQUIRE(test_tree.get(test_index_path));
    REQUIRE_FALSE(test_tree.get("a/b/c"));
}

TEST_CASE("tree: set update flags", "[tree]") {
    state tree;
    tree.set(test_index_path, true);
    REQUIRE(tree.get(test_index_path));
}

TEST_CASE("tree: serialization", "[tree]") {
    const path path = temp_directory_path() / "tree_serialization_test.lf";
    
    std::fstream file(path, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
    REQUIRE( file << write_as<tree_binary_format>(test_tree) );
    REQUIRE( file.seekg(0).good() );

    state d;
    REQUIRE( file >> read_as<tree_binary_format>(d) );

    cmp_tree(test_tree, d);
}

TEST_CASE("tree: deserialize with wrong signature", "[tree]") {
    log_tester t(INFO);

    std::stringstream ss("WRONGSIGNATUREHERE!!!");

    state d;
    REQUIRE_FALSE( ss >> read_as<tree_binary_format>(d) );
    REQUIRE( t.str().find("invalid file signature") != std::string::npos );
}

TEST_CASE("tree: deserialize must fail on empty stream", "[tree]") {
    state result; 
    std::stringstream ss;
    REQUIRE_FALSE( ss >> read_as<tree_binary_format>(result));
}

TEST_CASE("tree: deserialize must clear entries", "[tree]") {

    state result; 
    std::stringstream ss;

    state t1 = { true, { {"a", state { true }} } };
    REQUIRE( ss << write_as<tree_binary_format>(t1) );
    REQUIRE( ss >> read_as<tree_binary_format>(result) );
    ss.str("");
    ss.clear();

    REQUIRE(result.get("") == true);
    REQUIRE(result.get("a") == true);
    REQUIRE(result.get("b") == false);

    state t2 = { false, { {"b", state { true }} } };
    REQUIRE( ss << write_as<tree_binary_format>(t2) );
    REQUIRE( ss >> read_as<tree_binary_format>(result) );
    ss.str("");
    ss.clear();

    REQUIRE(result.get("") == false);
    REQUIRE(result.get("a") == false);
    REQUIRE(result.get("b") == true);
}

TEST_CASE("tree: set", "[tree]") {
    state tree;
    CHECK_FALSE( tree.set(false) );
    REQUIRE( tree.set(true) );
    CHECK_FALSE( tree.set(true) );
    REQUIRE( tree.set(false) );
}

TEST_CASE("tree: set must create node with default", "[tree]") {
    state tree;
    REQUIRE( tree.entries.empty() );

    CHECK( tree.set(test_index_path, false) );
    REQUIRE( tree.node(test_index_path) != nullptr );
    REQUIRE( tree.node(test_index_path)->data == false );
}

TEST_CASE("tree: set must keep node with default", "[tree]") {
    state tree;

    CHECK( tree.set(test_index_path, true) );
    REQUIRE( tree.get(test_index_path) == true );
    
    CHECK( tree.set(test_index_path, false) );
    REQUIRE_FALSE( tree.get(test_index_path) );
    REQUIRE( tree.node(test_index_path) != nullptr );
}

TEST_CASE("tree: set must preserve adjanced entries when removing redunant entries", "[tree]") {
    state tree;

    const path adjanced_path = test_index_path.parent_path() / "x.jpg";

    CHECK( tree.set(test_index_path, true) );
    CHECK( tree.set(adjanced_path, true) );
    REQUIRE( !tree.entries.empty() );
    
    CHECK( tree.set(test_index_path, false) );
    REQUIRE( tree.node(adjanced_path) != nullptr );
}

TEST_CASE("tree: set must keep entries with children", "[tree]") {
    const path intermediate_path = "a/b";
    const path nested_path = intermediate_path / "c/test.yaml";

    state tree;
    CHECK( tree.set(intermediate_path, true) );
    CHECK( tree.set(nested_path, true) );
    REQUIRE( tree.node(nested_path) != nullptr );

    CHECK( tree.set(intermediate_path, false) );
    REQUIRE_FALSE( tree.get(intermediate_path) );
    REQUIRE( tree.node(nested_path) != nullptr );
}

TEST_CASE("tree: remove", "[tree]") {
    
    const path intermediate_path = "a/b";
    const path nested_path = intermediate_path / "c/test.yaml";

    state tree;
    CHECK( tree.set(intermediate_path, true) );
    CHECK( tree.set(nested_path, true) );
    REQUIRE( tree.node(nested_path) != nullptr );

    CHECK( tree.remove(intermediate_path) );
    REQUIRE( tree.node(intermediate_path) == nullptr );
    REQUIRE( tree.node(nested_path) == nullptr );

}

TEST_CASE("tree: set must return false when not modified", "[tree]") {

    const path path = "a/b/c";
    
    state tree;
    REQUIRE( tree.set(path, true) );
    REQUIRE_FALSE( tree.set(path, true) );
    REQUIRE( tree.set(path.parent_path(), true) );
    REQUIRE_FALSE( tree.set(path.parent_path(), true) );

}

TEST_CASE("tree: remove if empty", "[tree]") {
    const path path = "a/b/c";

    state tree;
    CHECK( tree.set(path, true) );
    CHECK_FALSE( tree.remove(path.parent_path(), true) );
    REQUIRE( tree.node(path) != nullptr );
}

TEST_CASE("tree: remove if empty must set default value", "[tree]") {
    const path path = "a/b/c";

    state tree;
    CHECK( tree.set(path, true) );
    CHECK( tree.set(path.parent_path(), true) );
    CHECK( tree.remove(path.parent_path(), true) );
    REQUIRE( tree.node(path) != nullptr );
    REQUIRE( tree.node(path.parent_path()) != nullptr );
    REQUIRE_FALSE( tree.node(path.parent_path())->data );

}

TEST_CASE("tree: must keep unspecified inside recursive and shallow", "[tree]") {

    const path common = "a/b";
    const path p1 = common / "c";
    const path p2 = common / "d";

    lf::index index;
    CHECK( index.set(common, sync_mode::RECURSIVE) );
    CHECK( index.set(p1, sync_mode::SHALLOW) );
    CHECK( index.set(p2, sync_mode::IGNORE) );

    REQUIRE( index.node(common) != nullptr );
    REQUIRE( index.node(common)->data == sync_mode::RECURSIVE );

    REQUIRE( index.node(p1) != nullptr );
    REQUIRE( index.node(p1)->data == sync_mode::SHALLOW );
    
    REQUIRE( index.node(p2) != nullptr );
    REQUIRE( index.node(p2)->data == sync_mode::IGNORE );

}

TEST_CASE("tree: move", "[tree]") {

    lf::index index;
    index.set("a/b/c.txt", sync_mode::SHALLOW);

    // CHECK( index.move("a/b/c.txt", "x/y/z.txt") );
    REQUIRE( index.get("a/b/c.txt") == sync_mode::NONE );
    REQUIRE( index.get("x/y/z.txt") == sync_mode::SHALLOW );
}