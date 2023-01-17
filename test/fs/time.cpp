#include <catch2/catch_test_macros.hpp>

#include "fs/time.hpp"

#include <chrono>
#include <map>

namespace fs = std::filesystem;

using fs_time = fs::file_time_type;
using fs_duration = fs_time::duration;
using fs_rep = fs_duration::rep;

fs_time fs_tp(const fs_rep& r) {
    return fs_time(fs_duration(r));
}

std::map<fs_rep, fs_rep> ntfs_expectations = {
    { 200L, 200L },
    { 199L, 100L },
    { 100L, 100L },
    { 99L, 0L },
    { 1L, 0L },
    { 0L, 0L },
    { -1L,  -100L },
    { -99L,  -100L },
    { -100L,  -100L },
    { -101L,  -200L }
};


TEST_CASE("write_ntfs_timestamp", "[.time]") {

    fs::path test_ntfs_path = "/mnt/router/tmp/test.txt";

    for (const auto& e: ntfs_expectations) {
        fs_time set_tp = fs_tp(e.first);
        fs::last_write_time(test_ntfs_path, set_tp);
        fs_time cur_tp = fs::last_write_time(test_ntfs_path); 
        REQUIRE(cur_tp == fs_tp(e.second));
    }

}

TEST_CASE("last_write_time", "[time]") {
    
    fs::file_time_type t1 = fs_tp(123424231423423L);

    fs::file_time_type expected_t1 = fs_tp(123424231423400L);
    fs::file_time_type actual_t1 = lf::ntfs_last_write_time(t1);
    REQUIRE( actual_t1 == expected_t1 );

    fs::file_time_type t2 = fs_tp(-4767892066937254531L);
    fs::file_time_type expected_t2 = fs_tp(-4767892066937254600L);
    fs::file_time_type actual_t2 = lf::ntfs_last_write_time(t2);
    REQUIRE( actual_t2 == expected_t2 );

    for (const auto& e: ntfs_expectations) {
        REQUIRE( lf::ntfs_last_write_time(fs_tp(e.first)) == fs_tp(e.second) );
    }

}