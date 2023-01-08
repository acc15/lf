#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <fstream>
#include <map>

#include "state/state_tree.hpp"

#include "tree/print.hpp"
#include "tree/binary.hpp"

#include "io/log_tester.hpp"

using namespace lf;
using namespace std::filesystem;

const state_tree test_tree = { false, {
    { "a", state_tree { false, {
        { "01.json", state_tree { false } }, 
        { "02.yaml", state_tree { false } }
    }}},
    { "b", state_tree { false, {
        { "03.sql", state_tree { true } }, 
        { "04.jpg", state_tree { false } }
    }}},
    { "c", state_tree { false, {
        { "05.cpp", state_tree { false } }, 
        { "06.hpp", state_tree { true } }
    }}},
    { "d", state_tree { false, {
        { "07.txt", state_tree { true } }, 
        { "08.bin", state_tree { true } }
    }}},
    { "e", state_tree { true, {
        { "09.json", state_tree { false } }, 
        { "10.yaml", state_tree { false } }
    }}},
    { "f", state_tree { true, {
        { "11.sql", state_tree { true } }, 
        { "12.jpg", state_tree { false } }
    }}},
    { "g", state_tree { true, {
        { "13.cpp", state_tree { false } }, 
        { "14.hpp", state_tree { true } }
    }}},
    { "h", state_tree { true, {
        { "15.txt", state_tree { true } }, 
        { "16.bin", state_tree { true } }
    }}}
}};

const path test_index_path = "b/03.sql";

TEST_CASE("print", "[tree]") {
    std::stringstream ss;

    ss << std::boolalpha;
    tree_print<state_tree, tree_entry_name_order<state_tree>>::print(ss, test_tree);
    
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

template <tree_data T>
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

TEST_CASE("get", "[tree]") {
    REQUIRE(test_tree.get(test_index_path));
    REQUIRE_FALSE(test_tree.get("a/b/c"));
}

TEST_CASE("set update flags", "[tree]") {
    state_tree tree;
    tree.set(test_index_path, true);
    REQUIRE(tree.get(test_index_path));
}

TEST_CASE("serialization", "[tree]") {
    const path path = temp_directory_path() / "tree_serialization_test.lf";
    
    std::fstream file(path, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
    REQUIRE( file << with_ref_format<format::BINARY>(test_tree) );
    REQUIRE( file.seekg(0).good() );

    state_tree d;
    REQUIRE( file >> with_ref_format<format::BINARY>(d) );

    cmp_tree(test_tree, d);
}

TEST_CASE("deserialize with wrong signature", "[tree]") {
    log_tester t;

    std::stringstream ss("WRONGSIGNATUREHERE!!!");

    state_tree d;
    REQUIRE_FALSE( ss >> with_ref_format<format::BINARY>(d) );
    REQUIRE( t.str().find("invalid file signature") != std::string::npos );
}

TEST_CASE("deserialize must fail on empty stream", "[tree]") {
    state_tree result; 
    std::stringstream ss;
    REQUIRE_FALSE( ss >> with_ref_format<format::BINARY>(result));
}

TEST_CASE("deserialize must clear entries", "[tree]") {

    state_tree result; 
    std::stringstream ss;

    state_tree t1 = { true, { {"a", state_tree { true }} } };
    REQUIRE( ss << with_cref_format<format::BINARY>(t1) );
    REQUIRE( ss >> with_ref_format<format::BINARY>(result) );
    ss.str("");
    ss.clear();

    REQUIRE(result.get("") == true);
    REQUIRE(result.get("a") == true);
    REQUIRE(result.get("b") == false);

    state_tree t2 = { false, { {"b", state_tree { true }} } };
    REQUIRE( ss << with_cref_format<format::BINARY>(t2) );
    REQUIRE( ss >> with_ref_format<format::BINARY>(result) );
    ss.str("");
    ss.clear();

    REQUIRE(result.get("") == false);
    REQUIRE(result.get("a") == false);
    REQUIRE(result.get("b") == true);
}

TEST_CASE("set", "[tree]") {
    state_tree tree;
    CHECK_FALSE( tree.set(false) );
    REQUIRE( tree.set(true) );
    CHECK_FALSE( tree.set(true) );
    REQUIRE( tree.set(false) );
}

TEST_CASE("set must avoid creating entries with default flags only", "[tree]") {
    state_tree tree;
    REQUIRE( tree.entries.empty() );

    CHECK_FALSE( tree.set(test_index_path, false) );
    REQUIRE( tree.entries.empty() );
}

TEST_CASE("set must remove redunant entries", "[tree]") {
    state_tree tree;

    CHECK( tree.set(test_index_path, true) );
    REQUIRE( !tree.entries.empty() );
    
    CHECK( tree.set(test_index_path, false) );
    REQUIRE( tree.entries.empty() );
}

TEST_CASE("set must preserve adjanced entries when removing redunant entries", "[tree]") {
    state_tree tree;

    const path adjanced_path = test_index_path.parent_path() / "x.jpg";

    CHECK( tree.set(test_index_path, true) );
    CHECK( tree.set(adjanced_path, true) );
    REQUIRE( !tree.entries.empty() );
    
    CHECK( tree.set(test_index_path, false) );
    REQUIRE( tree.node(adjanced_path) != nullptr );
}

TEST_CASE("set must preserve intermediate non-default nodes", "[tree]") {
    path intermediate_path = path("a") / "b";
    path file_path = intermediate_path / "c" / "file.txt";

    state_tree tree;
    CHECK( tree.set(intermediate_path, true) );
    CHECK( tree.set(file_path, true) );
    REQUIRE( tree.node(file_path) != nullptr );

    CHECK( tree.set(file_path, false) );
    REQUIRE( tree.node(file_path) == nullptr );

    auto intermediate_entry = tree.node(intermediate_path);
    REQUIRE( intermediate_entry != nullptr );
    REQUIRE( intermediate_entry->entries.empty() );
}

TEST_CASE("set must keep entries with children", "[tree]") {
    const path intermediate_path = "a/b";
    const path nested_path = intermediate_path / "c/test.yaml";

    state_tree tree;
    CHECK( tree.set(intermediate_path, true) );
    CHECK( tree.set(nested_path, true) );
    REQUIRE( tree.node(nested_path) != nullptr );

    CHECK( tree.set(intermediate_path, false) );
    REQUIRE_FALSE( tree.get(intermediate_path) );
    REQUIRE( tree.node(nested_path) != nullptr );
}

TEST_CASE("remove must remove entries with children", "[tree]") {
    
    const path intermediate_path = "a/b";
    const path nested_path = intermediate_path / "c/test.yaml";

    state_tree tree;
    CHECK( tree.set(intermediate_path, true) );
    CHECK( tree.set(nested_path, true) );
    REQUIRE( tree.node(nested_path) != nullptr );

    CHECK( tree.remove(intermediate_path) );
    REQUIRE( tree.entries.empty() );

}

TEST_CASE("set must keep entries with single children", "[tree]") {

    const path common_path = "a/b";
    const path p1 = common_path / "x/y";
    const path p2 = common_path / "z/w";

    state_tree tree;
    CHECK( tree.set(p1, true) );
    CHECK_FALSE( tree.set(p2, false) );

    REQUIRE( tree.node(p1) != nullptr );

}
