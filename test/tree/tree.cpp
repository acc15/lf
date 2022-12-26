#include <catch2/catch_test_macros.hpp>

#include <iostream>

#include "tree/tree.hpp"
#include "tree/tree_print.hpp"
#include "tree/tree_binary.hpp"

using namespace lf;
using namespace std::filesystem;

using str_tree = tree<std::string>;

const str_tree test_tree = {
    .data = "root",
    .entries = {
        { 
            "a", 
            str_tree {
                .data = "a",
                .entries = {
                    {
                        "b.json",
                        str_tree {
                            .data = "b.json"
                        }
                    },
                    {
                        "c.yaml",
                        str_tree {
                            .data = "c.yaml"
                        }
                    }
                }
            }
        },
        {
            "d",
            str_tree {
                .data = "d",
                .entries = {
                    {
                        "e.sql",
                        str_tree {
                            .data = "e.sql"
                        }
                    },
                    {
                        "f.jpg",
                        str_tree {
                            .data = "f.jpg"
                        }
                    }
                    
                }
            }
        },
        {
            "g",
            str_tree {
                .data = "g",
                .entries = {
                    {
                        "h.mp3",
                        str_tree {
                            .data = "h.mp3"
                        }
                    },
                    {
                        "i.mp4",
                        str_tree {
                            .data = "i.mp4"
                        }
                    }
                }
            }
        }
    }
};

const path test_index_path = path("a") / "c.yaml";

TEST_CASE("print", "[tree]") {
    std::stringstream ss;
    ss << test_tree;
    REQUIRE(ss.str() == 
        "<root> [root]\n"
        "├── g [g]\n"
        "│   ├── i.mp4 [i.mp4]\n"
        "│   └── h.mp3 [h.mp3]\n"
        "├── d [d]\n"
        "│   ├── f.jpg [f.jpg]\n"
        "│   └── e.sql [e.sql]\n"
        "└── a [a]\n"
        "    ├── c.yaml [c.yaml]\n"
        "    └── b.json [b.json]\n"
    );
}

TEST_CASE("get", "[tree]") {
    REQUIRE(test_tree.get(test_index_path) == "c.yaml");
    REQUIRE(test_tree.get("a/b/c") == "");
}

TEST_CASE("set update flags", "[tree]") {
    str_tree tree;
    tree.set(test_index_path, "abc");
    REQUIRE( tree.get(test_index_path) == "abc" );
}

TEST_CASE("set must avoid creating entries with default flags only", "[tree]") {
    str_tree tree;
    REQUIRE( tree.entries.empty() );

    tree.set(test_index_path, "");
    REQUIRE( tree.entries.empty() );
}

TEST_CASE("set must remove redunant entries", "[tree]") {
    str_tree tree;

    tree.set(test_index_path, "abc");
    REQUIRE( !tree.entries.empty() );
    
    tree.set(test_index_path, "");
    REQUIRE( tree.entries.empty() );
}

TEST_CASE("set must preserve adjanced entries when removing redunant entries", "[tree]") {
    str_tree tree;

    const path adjanced_path = test_index_path.parent_path() / "x.jpg";

    tree.set(test_index_path, "abc");
    tree.set(adjanced_path, "xyz");
    REQUIRE( !tree.entries.empty() );
    
    tree.set(test_index_path, "");
    REQUIRE( tree.node(adjanced_path) != nullptr );
}

TEST_CASE("set must preserve intermediate non-default nodes", "[tree]") {
    path intermediate_path = path("a") / "b";
    path file_path = intermediate_path / "c" / "file.txt";

    str_tree tree;
    tree.set(intermediate_path, "abc");
    tree.set(file_path, "xyz");
    REQUIRE( tree.node(file_path) != nullptr );

    tree.set(file_path, "");
    REQUIRE( tree.node(file_path) == nullptr );

    auto intermediate_entry = tree.node(intermediate_path);
    REQUIRE(intermediate_entry != nullptr);
    REQUIRE(intermediate_entry->entries.empty());
}

TEST_CASE("set must keep entries with children", "[tree]") {
    const path intermediate_path = "a/b";
    const path nested_path = intermediate_path / "c/test.yaml";

    str_tree tree;
    tree.set(intermediate_path, "abc");
    tree.set(nested_path, "xyz");
    REQUIRE(tree.node(nested_path) != nullptr);

    tree.set(intermediate_path, "");
    REQUIRE(tree.node(nested_path) != nullptr);
}

TEST_CASE("set must keep entries with single children", "[tree]") {

    const path common_path = "a/b";
    const path p1 = common_path / "x/y";
    const path p2 = common_path / "z/w";

    str_tree tree;
    tree.set(p1, "abc");
    tree.set(p2, "");

    REQUIRE( tree.node(p1) != nullptr );

}


// a 
// b (0) -> x (0) -> y (1) -> z (0) -> w (1) 
// c

// a 
// b(0) -> [x(1)] -> y(0) -> z(0) -> w(1)
// c

