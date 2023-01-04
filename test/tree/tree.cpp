#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <fstream>
#include <map>

#include "tree/tree.hpp"
#include "tree/print.hpp"
#include "tree/binary.hpp"

#include "io/log_tester.hpp"

using namespace lf;
using namespace std::filesystem;

using bool_tree = tree<bool>;

struct bool_root : bool_tree {
    
    static const char file_signature[];
    static const uint8_t file_version;

};

const char bool_root::file_signature[] = "TEST";
const uint8_t bool_root::file_version = 0;

namespace lf {

    std::ostream& operator<<(std::ostream& s, with_format<format::TREE, const bool&> v) {
        return s << v.value;
    }

    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const bool&> v) {
        return s.put(v.value);
    }

    std::istream& operator>>(std::istream& s, with_format<format::BINARY, bool&> v) {
        std::uint8_t value;
        if (s >> value) {
            v.value = static_cast<bool>(value);
        }
        return s;
    }

}

const bool_root test_tree = 
{{ false, {
    { "a", bool_tree { false, {
        { "01.json", bool_tree { false } }, 
        { "02.yaml", bool_tree { false } }
    }}},
    { "b", bool_tree { false, {
        { "03.sql", bool_tree { true } }, 
        { "04.jpg", bool_tree { false } }
    }}},
    { "c", bool_tree { false, {
        { "05.cpp", bool_tree { false } }, 
        { "06.hpp", bool_tree { true } }
    }}},
    { "d", bool_tree { false, {
        { "07.txt", bool_tree { true } }, 
        { "08.bin", bool_tree { true } }
    }}},
    { "e", bool_tree { true, {
        { "09.json", bool_tree { false } }, 
        { "10.yaml", bool_tree { false } }
    }}},
    { "f", bool_tree { true, {
        { "11.sql", bool_tree { true } }, 
        { "12.jpg", bool_tree { false } }
    }}},
    { "g", bool_tree { true, {
        { "13.cpp", bool_tree { false } }, 
        { "14.hpp", bool_tree { true } }
    }}},
    { "h", bool_tree { true, {
        { "15.txt", bool_tree { true } }, 
        { "16.bin", bool_tree { true } }
    }}}
}}};

const path test_index_path = "b/03.sql";

TEST_CASE("print", "[tree]") {
    std::stringstream ss;

    ss << std::boolalpha;
    tree_print<bool_tree, tree_entry_name_order<bool_tree>>::print(ss, test_tree);
    
    const std::string str = ss.str();
    REQUIRE(str == 
        "<root> [false]\n"
        "├── a [false]\n"
        "│   ├── 01.json [false]\n"
        "│   └── 02.yaml [false]\n"
        "├── b [false]\n"
        "│   ├── 03.sql [true]\n"
        "│   └── 04.jpg [false]\n"
        "├── c [false]\n"
        "│   ├── 05.cpp [false]\n"
        "│   └── 06.hpp [true]\n"
        "├── d [false]\n"
        "│   ├── 07.txt [true]\n"
        "│   └── 08.bin [true]\n"
        "├── e [true]\n"
        "│   ├── 09.json [false]\n"
        "│   └── 10.yaml [false]\n"
        "├── f [true]\n"
        "│   ├── 11.sql [true]\n"
        "│   └── 12.jpg [false]\n"
        "├── g [true]\n"
        "│   ├── 13.cpp [false]\n"
        "│   └── 14.hpp [true]\n"
        "└── h [true]\n"
        "    ├── 15.txt [true]\n"
        "    └── 16.bin [true]\n"
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
    bool_tree tree;
    tree.set(test_index_path, true);
    REQUIRE(tree.get(test_index_path));
}

TEST_CASE("serialization", "[tree]") {
    const path path = temp_directory_path() / "tree_serialization_test.lf";
    
    std::fstream file(path, std::ios_base::in | std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
    REQUIRE( file << with_ref_format<format::BINARY>(test_tree) );
    REQUIRE( file.seekg(0).good() );

    bool_root d;
    REQUIRE( file >> with_ref_format<format::BINARY>(d) );

    cmp_tree(test_tree, d);
}

TEST_CASE("deserialize with wrong signature", "[tree]") {
    log_tester t;

    std::stringstream ss("WRONGSIGNATUREHERE!!!");

    bool_root d;
    REQUIRE_FALSE( ss >> with_ref_format<format::BINARY>(d) );
    REQUIRE( t.str().find("invalid file signature") != std::string::npos );
}

TEST_CASE("deserialize must fail on empty stream", "[tree]") {
    bool_root result; 
    std::stringstream ss;
    REQUIRE_FALSE( ss >> with_ref_format<format::BINARY>(result));
}

TEST_CASE("deserialize must clear entries", "[tree]") {

    bool_root result; 
    std::stringstream ss;

    bool_root t1 = {{ true, { {"a", bool_tree { true }} } }};
    REQUIRE( ss << with_cref_format<format::BINARY>(t1) );
    REQUIRE( ss >> with_ref_format<format::BINARY>(result) );
    ss.str("");
    ss.clear();

    REQUIRE(result.get("") == true);
    REQUIRE(result.get("a") == true);
    REQUIRE(result.get("b") == false);

    bool_root t2 = {{ false, { {"b", bool_tree { true }} } }};
    REQUIRE( ss << with_cref_format<format::BINARY>(t2) );
    REQUIRE( ss >> with_ref_format<format::BINARY>(result) );
    ss.str("");
    ss.clear();

    REQUIRE(result.get("") == false);
    REQUIRE(result.get("a") == false);
    REQUIRE(result.get("b") == true);
}

TEST_CASE("set", "[tree]") {
    bool_tree tree;
    CHECK_FALSE( tree.set(false) );
    REQUIRE( tree.set(true) );
    CHECK_FALSE( tree.set(true) );
    REQUIRE( tree.set(false) );
}

TEST_CASE("set must avoid creating entries with default flags only", "[tree]") {
    bool_tree tree;
    REQUIRE( tree.entries.empty() );

    CHECK_FALSE( tree.set(test_index_path, false) );
    REQUIRE( tree.entries.empty() );
}

TEST_CASE("set must remove redunant entries", "[tree]") {
    bool_tree tree;

    CHECK( tree.set(test_index_path, true) );
    REQUIRE( !tree.entries.empty() );
    
    CHECK( tree.set(test_index_path, false) );
    REQUIRE( tree.entries.empty() );
}

TEST_CASE("set must preserve adjanced entries when removing redunant entries", "[tree]") {
    bool_tree tree;

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

    bool_tree tree;
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

    bool_tree tree;
    CHECK( tree.set(intermediate_path, true) );
    CHECK( tree.set(nested_path, true) );
    REQUIRE( tree.node(nested_path) != nullptr );

    CHECK( tree.set(intermediate_path, false) );
    REQUIRE_FALSE( tree.get(intermediate_path) );
    REQUIRE( tree.node(nested_path) != nullptr );
}

TEST_CASE("set must remove entries with children when remove_default_subtree", "[tree]") {
    
    const path intermediate_path = "a/b";
    const path nested_path = intermediate_path / "c/test.yaml";

    bool_tree tree;
    CHECK( tree.set(intermediate_path, true) );
    CHECK( tree.set(nested_path, true) );
    REQUIRE( tree.node(nested_path) != nullptr );

    CHECK( tree.set(intermediate_path, false, false) );
    REQUIRE( tree.entries.empty() );

}


TEST_CASE("set must keep entries with single children", "[tree]") {

    const path common_path = "a/b";
    const path p1 = common_path / "x/y";
    const path p2 = common_path / "z/w";

    bool_tree tree;
    CHECK( tree.set(p1, true) );
    CHECK_FALSE( tree.set(p2, false) );

    REQUIRE( tree.node(p1) != nullptr );

}
