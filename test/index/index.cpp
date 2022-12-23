#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <cstdlib>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

#include <fmt/core.h>

#include <index/index.hpp>
#include <io/with_format.hpp>

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

void cmp_index(const leafsync::index_root& l, const leafsync::index_root& r) {
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

    const index_flags s = { 
        .mode = GENERATE( index_sync_mode::NONE, index_sync_mode::SHALLOW, index_sync_mode::RECURSIVE ), 
        .sync = GENERATE( false, true ) 
    };

    REQUIRE( (ss << formatted_as<format::BINARY>(s)).good() );

    index_flags d;
    REQUIRE( (ss >> formatted_as<format::BINARY>(d)).good() );

    cmp_index_flags(s, d);
}

index_flags rand_flags() {
    return index_flags{
        .mode = static_cast<index_sync_mode>(rand() % 3),
        .sync = rand() % 2 != 0,
    };
}

std::string rand_name() {
    std::string result;
    result.resize(2 + rand() % 9);
    for (auto& c : result) {
        c = 'a' + rand() % 26;
    }
    return result;
}

std::vector<index_entry> rand_entries() {
    std::vector<index_entry> result;
    int size = rand() % 3;
    result.resize(size);
    for (auto& c : result) {
        c.name = rand_name();
        c.flags = rand_flags();
        c.entries = rand_entries();
    }
    return result;
}

TEST_CASE("serialization", "[index]") {
    
    const auto path = std::filesystem::temp_directory_path() / "index_serialization_test.index";
    std::fstream file(path, ios_base::in | ios_base::out | ios_base::binary | ios_base::trunc );

    srand(7);

    const index_root s {
        .flags = rand_flags(),
        .entries = rand_entries()
    };
    std::cout << s;


    REQUIRE( file << formatted_as<format::BINARY>(s) );
    REQUIRE( file.seekg(0).good() );

    index_root d;
    errors err(data_location { .source = path.string() });

    REQUIRE( file >> formatted_as<format::BINARY>(d, err) );
    std::cout << d;

    cmp_index(s, d);


}
