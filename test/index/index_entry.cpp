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

TEST_CASE("serialization", "[index_root]") {
    const auto path = filesystem::temp_directory_path() / "index_serialization_test.index";
    fstream file(path, ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc );
    REQUIRE( file << with_ref_format<format::BINARY>(test_index) );
    REQUIRE( file.seekg(0).good() );

    index_entry d;
    errors err(data_location { .source = path.string() });

    REQUIRE( file >> with_ref_format<format::BINARY>(d, err) );

    cmp_index_entry(test_index, d);
}

TEST_CASE("get", "[index_root]") {
    const auto flags = test_index.get(std::filesystem::path("a") / "c.yaml");
    cmp_index_flags(flags, index_flags {.mode = index_sync_mode::SHALLOW, .sync = true });
}

TEST_CASE("set", "[index_root]") {
    index_entry index;
    index_flags flags = { .mode = index_sync_mode::NONE, .sync = true };

    index.set(std::filesystem::path("a") / "b" / "c" / "d.yaml", flags);
    cmp_index_flags(index.entries["a"].entries["b"].entries["c"].entries["d.yaml"].flags, flags);
}