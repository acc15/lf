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
    { -1L, -100L },
    { -99L, -100L },
    { -100L, -100L },
    { -101L, -200L },
    { 123424231423423L, 123424231423400L },
    { -4767892066937254531L, -4767892066937254600L }
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
    for (const auto& e: ntfs_expectations) {
        fs_time set = fs_tp(e.first);
        INFO("time set " << set.time_since_epoch());
        fs_time actual = lf::ntfs_last_write_time(set);
        fs_time expect = fs_tp(e.second);
        REQUIRE( actual == expect );
    }
}