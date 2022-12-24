#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <io/with_format.hpp>
#include <index/index_root.hpp>

#include "index_cmp.hpp"

using namespace lf;
using namespace std;

TEST_CASE("serialization", "[index_root]") {
    
    const auto path = filesystem::temp_directory_path() / "index_serialization_test.index";
    fstream file(path, ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc );

    const index_root s {
        .flags = { .mode = index_sync_mode::NONE, .sync = false },
        .entries = {
            index_entry {
                .name = "a",
                .flags = { .mode = index_sync_mode::NONE, .sync = false },
                .entries = {
                    index_entry {
                        .name = "b.json",
                        .flags = { .mode = index_sync_mode::SHALLOW, .sync = false },
                    },
                    index_entry {
                        .name = "c.yaml",
                        .flags = { .mode = index_sync_mode::SHALLOW, .sync = true },
                    }
                }
            },
            index_entry {
                .name = "d",
                .flags = { .mode = index_sync_mode::RECURSIVE, .sync = false },
                .entries = {
                    index_entry {
                        .name = "e.sql",
                        .flags = { .mode = index_sync_mode::RECURSIVE, .sync = false },
                    },
                    index_entry {
                        .name = "f.jpg",
                        .flags = { .mode = index_sync_mode::RECURSIVE, .sync = true },
                    }
                }
            },
            index_entry {
                .name = "g",
                .flags = { .mode = index_sync_mode::SHALLOW, .sync = true },
                .entries = {
                    index_entry {
                        .name = "h.mp3",
                        .flags = { .mode = index_sync_mode::SHALLOW, .sync = true },
                    },
                    index_entry {
                        .name = "i.mp4",
                        .flags = { .mode = index_sync_mode::SHALLOW, .sync = true },
                    }
                }
            },
        }
    };

    REQUIRE( file << with_ref_format<format::BINARY>(s) );
    REQUIRE( file.seekg(0).good() );

    index_root d;
    errors err(data_location { .source = path.string() });

    REQUIRE( file >> with_ref_format<format::BINARY>(d, err) );
    cmp_index(s, d);

}
