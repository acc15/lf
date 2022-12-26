#include <catch2/catch_test_macros.hpp>

#include "tree/tree.hpp"
#include "tree/tree_print.hpp"

using namespace lf;

using test_node = tree<std::string>;

const test_node test_index = {
    .data = "root",
    .entries = {
        { 
            "a", 
            test_node {
                .data = "a",
                .entries = {
                    {
                        "b.json",
                        test_node {
                            .data = "b.json"
                        }
                    },
                    {
                        "c.yaml",
                        test_node {
                            .data = "c.yaml"
                        }
                    }
                }
            }
        },
        {
            "d",
            test_node {
                .data = "d",
                .entries = {
                    {
                        "e.sql",
                        test_node {
                            .data = "e.sql"
                        }
                    },
                    {
                        "f.jpg",
                        test_node {
                            .data = "f.jpg"
                        }
                    }
                    
                }
            }
        },
        {
            "g",
            test_node {
                .data = "g",
                .entries = {
                    {
                        "h.mp3",
                        test_node {
                            .data = "h.mp3"
                        }
                    },
                    {
                        "i.mp4",
                        test_node {
                            .data = "i.mp4"
                        }
                    }
                }
            }
        }
    }
};

TEST_CASE("print", "[tree]") {

}