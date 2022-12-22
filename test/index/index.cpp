#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <filesystem>
#include <fstream>
#include <ios>
#include <sstream>

#include <fmt/core.h>

#include <index/index.hpp>

using namespace leafsync;
using namespace std;

void cmp_index_flags(const index_flags& l, const index_flags& r) {
    CHECK( (l.mode == r.mode) );
    CHECK( (l.sync == r.sync) );
}

void cmp_index_entry(const index_entry& l, const index_entry& r);

void cmp_index_entries(const std::vector<index_entry>& l, const std::vector<index_entry>& r) {
    CHECK( l.size() == r.size() );
    for (size_t i = 0; i < std::min(l.size(), r.size()); i++) {
        cmp_index_entry(l[i], r[i]);
    }
}

void cmp_index_entry(const index_entry& l, const index_entry& r) {
    CHECK( l.name == r.name );
    cmp_index_flags(l.flags, r.flags);
    cmp_index_entries(l.entries, r.entries);
}

void cmp_index(const leafsync::index& l, const leafsync::index& r) {
    cmp_index_flags(l.flags, r.flags);
    cmp_index_entries(l.entries, r.entries);
}

TEST_CASE("set", "[index_flags]") {
    index_flags flags = {
        .mode = index_sync_mode::RECURSIVE,
        .sync = true
    };
    std::uint8_t& byte = reinterpret_cast<std::uint8_t&>( flags );
    REQUIRE( byte == 0b110 );
}

TEST_CASE("serialization", "[index_flags]") {
    std::stringstream ss;

    index_flags s = { 
        .mode = GENERATE( index_sync_mode::NONE, index_sync_mode::SHALLOW, index_sync_mode::RECURSIVE ), 
        .sync = GENERATE( false, true ) 
    };

    REQUIRE( (ss << s).good() );

    index_flags d;
    REQUIRE( (ss >> d).good() );

    cmp_index_flags(s, d);
}

TEST_CASE("serialization", "[index]") {
    
    const auto path = std::filesystem::temp_directory_path() / "index_serialization_test.index";
    fmt::print("tmp file: {}\n", path.string());

    
    std::fstream file(std::filesystem::temp_directory_path() / "index_serialization_test.index", ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc );

    leafsync::index s {
        .flags = { .mode = index_sync_mode::NONE, .sync = false },
        .entries = {
            index_entry { 
                .name = "a", 
                .flags { .mode = index_sync_mode::NONE, .sync = false },
                .entries = {
                    index_entry { .name = "test.json", .flags { .mode = index_sync_mode::SHALLOW, .sync = false } },
                    index_entry { .name = "test.yaml", .flags { .mode = index_sync_mode::SHALLOW, .sync = true } }
                }
            },
            index_entry { 
                .name = "b", 
                .flags { .mode = index_sync_mode::RECURSIVE, .sync = true },
                .entries = {
                    index_entry { 
                        .name = "c", 
                        .flags { .mode = index_sync_mode::RECURSIVE, .sync = true },
                        .entries = {
                            index_entry { .name = "list.txt", .flags { .mode = index_sync_mode::RECURSIVE, .sync = true } },
                            index_entry { .name = "list.xml", .flags { .mode = index_sync_mode::RECURSIVE, .sync = true } }
                        }
                    }
                }
            }
        }
    };

    REQUIRE( file << s );
    
    file.clear();
    REQUIRE( file.seekg(0).good() );

    leafsync::index d;
    REQUIRE( file >> d );

    cmp_index(s, d);

}
