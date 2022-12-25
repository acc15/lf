#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <io/with_format.hpp>

#include "index/index_entry.hpp"
#include "index/index_flags.hpp"
#include "index/index_sync_mode.hpp"
#include "index_cmp.hpp"

using namespace lf;
using namespace std;
using namespace std::filesystem;

const index_entry test_index = {
    .flags = { .mode = index_sync_mode::NONE, .sync = false },
    .entries = {
        { 
            "a", 
            index_entry {
                .flags = { .mode = index_sync_mode::NONE, .sync = false },
                .entries = {
                    {
                        "b.json",
                        index_entry {
                            .flags = { .mode = index_sync_mode::SHALLOW, .sync = false },
                        }
                    },
                    {
                        "c.yaml",
                        index_entry {
                            .flags = { .mode = index_sync_mode::SHALLOW, .sync = true },
                        }
                    }
                }
            }
        },
        {
            "d",
            index_entry {
                .flags = { .mode = index_sync_mode::RECURSIVE, .sync = false },
                .entries = {
                    {
                        "e.sql",
                        index_entry {
                            .flags = { .mode = index_sync_mode::RECURSIVE, .sync = false },
                        }
                    },
                    {
                        "f.jpg",
                        index_entry {
                            .flags = { .mode = index_sync_mode::RECURSIVE, .sync = true },
                        }
                    }
                    
                }
            }
        },
        {
            "g",
            index_entry {
                .flags = { .mode = index_sync_mode::SHALLOW, .sync = true },
                .entries = {
                    {
                        "h.mp3",
                        index_entry {
                            .flags = { .mode = index_sync_mode::SHALLOW, .sync = true },
                        }
                    },
                    {
                        "i.mp4",
                        index_entry {
                            .flags = { .mode = index_sync_mode::SHALLOW, .sync = true },
                        }
                    }
                }
            }
        }
    }
};

const path test_index_path = path("a") / "c.yaml";

const index_flags none_flags = { .mode = index_sync_mode::NONE };
const index_flags shallow_flags = { .mode = index_sync_mode::SHALLOW };
const index_flags recursive_flags = { .mode = index_sync_mode::RECURSIVE };

TEST_CASE("serialization", "[index_root]") {
    const auto path = temp_directory_path() / "index_serialization_test.index";
    fstream file(path, ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc );
    REQUIRE( file << with_ref_format<format::BINARY>(test_index) );
    REQUIRE( file.seekg(0).good() );

    index_entry d;
    errors err(data_location { .source = path.string() });

    REQUIRE( file >> with_ref_format<format::BINARY>(d, err) );

    cmp_index_entry(test_index, d);
}

TEST_CASE("get", "[index_entry]") {
    cmp_index_flags(test_index.get(test_index_path), index_flags {.mode = index_sync_mode::SHALLOW, .sync = true });
    cmp_index_flags(test_index.get(path("x")), none_flags);
}

TEST_CASE("set update flags", "[index_entry]") {
    index_entry index;
    index.set(test_index_path, shallow_flags);
    cmp_index_flags(index.get(test_index_path), shallow_flags);
}

TEST_CASE("set must avoid creating entries with default flags only", "[index_entry]") {
    index_entry index;
    REQUIRE( index.entries.empty() );

    index.set(test_index_path, none_flags);
    REQUIRE( index.entries.empty() );
}

TEST_CASE("set must remove redunant entries", "[index_entry]") {
    index_entry index;

    index.set(test_index_path, shallow_flags);
    REQUIRE( !index.entries.empty() );
    
    index.set(test_index_path, none_flags);
    REQUIRE( index.entries.empty() );
}

TEST_CASE("set must preserve adjanced entries when removing redunant entries", "[index_entry]") {
    index_entry index;

    const path adjanced_path = test_index_path.parent_path() / "x.jpg";

    index.set(test_index_path, shallow_flags);
    index.set(adjanced_path, shallow_flags);
    REQUIRE( !index.entries.empty() );
    
    index.set(test_index_path, none_flags);
    REQUIRE( index.entry(adjanced_path) != nullptr );

}

TEST_CASE("set must preserve intermediate shallow directory", "[index_entry]") {

    path shallow_path = path("a") / "b";
    path file_path = shallow_path / "c" / "file.txt";

    index_entry index;
    index.set(shallow_path, shallow_flags);
    index.set(file_path, shallow_flags);
    REQUIRE( index.entry(file_path) != nullptr );

    index.set(file_path, none_flags);
    REQUIRE( index.entry(file_path) == nullptr );

    auto shallow_entry = index.entry(shallow_path);
    REQUIRE(shallow_entry != nullptr);
    REQUIRE(shallow_entry->entries.empty());

}

TEST_CASE("set must add default flags in shallow directory", "[index_entry]") {
    index_entry index;
    
    index.set(test_index_path.parent_path(), shallow_flags);
    index.set(test_index_path, none_flags);
    
    cmp_index_flags(index.get(test_index_path.parent_path()), shallow_flags);
    REQUIRE( index.entry(test_index_path) != nullptr );
}

TEST_CASE("set must add default flags in recursive directory", "[index_entry]") {
    index_entry index;

    const path deep_path = test_index_path.parent_path() / "x" / "y" / "z.jpg";

    index.set(test_index_path.parent_path(), recursive_flags);
    index.set(deep_path, none_flags);
    
    cmp_index_flags(index.get(test_index_path.parent_path()), recursive_flags);
    
    const auto e = index.entry(deep_path); 
    REQUIRE( e != nullptr );
    cmp_index_flags(e->flags, none_flags);
}

TEST_CASE("set must remove whole tree", "[index_entry]") {
    const path recursive_dir = "a/b";
    const path recursive_file = recursive_dir / "c/test.yaml";

    index_entry index;
    index.set(recursive_dir, recursive_flags);
    index.set(recursive_file, shallow_flags);
    REQUIRE(index.entry(recursive_file) != nullptr);

    index.set(recursive_dir, none_flags);
    REQUIRE(index.entries.empty());
}