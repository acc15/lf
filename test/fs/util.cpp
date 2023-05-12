#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "fs/util.hpp"

#include "log/log_tester.hpp"
#include "test_util.hpp"

using namespace lf;
namespace fs = std::filesystem;

TEST_CASE("path: absolute_path", "[path]") {
    const auto cd = fs::current_path();
    const auto this_dir = cd.filename().string();

    REQUIRE( absolute_path("") == cd );
    REQUIRE( absolute_path(".") == cd );
    REQUIRE( absolute_path("./") == cd );
    REQUIRE( absolute_path("../") == cd.parent_path() );
    REQUIRE( absolute_path("../a.txt") == cd.parent_path() / "a.txt" );
    REQUIRE( absolute_path("./a.txt") == cd / "a.txt" );
    REQUIRE( absolute_path("./a.txt") == cd / "a.txt" );
    REQUIRE( absolute_path(fs::path("..") / this_dir / "a.txt") == cd / "a.txt" );
}

TEST_CASE("path: relative_path", "[path]") {
    const fs::path root_path = fs::path("a");
    const fs::path test_path = fs::path("b") / "c";

    REQUIRE( relative_path("", "") == "" );
    REQUIRE( relative_path(root_path / test_path, root_path) == test_path );
    REQUIRE_FALSE( relative_path(test_path, fs::current_path()) );
    REQUIRE_FALSE( relative_path(test_path, fs::path("x") / "y" / "z") );
}

TEST_CASE("path: is_subpath", "[path]") {
    REQUIRE( is_subpath("a/b/c", "a/b") );
    REQUIRE_FALSE( is_subpath("a/b/c", "b") );
    REQUIRE_FALSE( is_subpath("a", "a/b/c") );
    REQUIRE_FALSE( is_subpath(test_root_path / "a", "a") );
    REQUIRE( is_subpath(test_root_path / "a", test_root_path / "a") );
}

TEST_CASE("path: create_parent_dirs", "[path]") {
    fs::path p1 = "abc";
    REQUIRE_FALSE( p1.has_parent_path() );
    REQUIRE( p1.parent_path().empty() );
    REQUIRE_FALSE( lf::create_parent_dirs(p1.parent_path()) );

    fs::path p2 = p1 / "xyz";
    REQUIRE( p2.has_parent_path() );
}

TEST_CASE("path: join_path", "[path]") {
    REQUIRE( join_path("abc", fs::path()) == fs::path("abc") );
    REQUIRE( join_path("abc", "xyz") == fs::path("abc") / "xyz" );
}

TEST_CASE("path: check_move current_path to anywhere", "[path]") {
    lf::log_tester t;
    REQUIRE( !check_move(fs::current_path(), fs::current_path().parent_path(), true) );
    REQUIRE( t.contains("current directory") );
}

TEST_CASE("path: check_move same path", "[path]") {
    lf::log_tester t;
    REQUIRE( !check_move(test_path1, test_path1, true) );
    REQUIRE( t.contains("itself") );
}

TEST_CASE("path: check_move to itself", "[path]") {
    lf::log_tester t;
    REQUIRE( !check_move(test_path1, test_path1 / "d", true) );
    REQUIRE( t.contains("itself") );
}

TEST_CASE("path: check_move non-existent", "[path]") {
    lf::log_tester t;
    REQUIRE( !check_move(test_path1, test_path2, true) );
    REQUIRE( t.contains("nothing to move") );
}

TEST_CASE("path: check_move file to non-existent", "[path]") {
    lf::cd_changer cd = create_temp_test_dir();
    write_text(test_path1, "abc");

    const auto result = check_move(test_path1, test_path2, true);
    REQUIRE( result );
    REQUIRE( result->first == fs::current_path() / test_path1 );
    REQUIRE( result->second == fs::current_path() / test_path2 );
}

TEST_CASE("path: check_move dir to non-existent", "[path]") {
    lf::cd_changer cd = create_temp_test_dir();
    fs::create_directories(test_path1);

    const auto result = check_move(test_path1, test_path2, true);
    REQUIRE( result );
    REQUIRE( result->first == fs::current_path() / test_path1 );
    REQUIRE( result->second == fs::current_path() / test_path2 );
}

TEST_CASE("path: check_move file to dir", "[path]") {
    lf::cd_changer cd = create_temp_test_dir();
    write_text(test_path1, "abc");
    fs::create_directories(test_path2);

    const auto result = check_move(test_path1, test_path2, true);
    REQUIRE( result );
    REQUIRE( result->first == fs::current_path() / test_path1 );
    REQUIRE( result->second == fs::current_path() / test_path2 / test_path1.filename() );
}

TEST_CASE("path: check_move file over dir", "[path]") {
    lf::log_tester t;
    lf::cd_changer cd = create_temp_test_dir();
    write_text(test_path1, "abc");
    fs::create_directories(test_path2 / test_path1.filename());
    REQUIRE_FALSE( check_move(test_path1, test_path2, true) );
    REQUIRE( t.contains("can't move") );
}

TEST_CASE("path: check_move dir over file", "[path]") {
    lf::log_tester t;
    lf::cd_changer cd = create_temp_test_dir();
    fs::create_directories(test_path1);
    write_text(test_path2, "abc");
    REQUIRE_FALSE( check_move(test_path1, test_path2, true) );
    REQUIRE( t.contains("can't move") );
}

TEST_CASE("path: move_path file to file", "[path]") {
    lf::cd_changer cd = create_temp_test_dir();
    write_text(test_path1, "abc");
    write_text(test_path2, "xyz");

    move_path(test_path1, test_path2);
    REQUIRE( !fs::exists(test_path1) );
    REQUIRE( read_text(test_path2) == "abc" );
}

TEST_CASE("path: move_path file to dir", "[path]") {
    const bool dest_exist = GENERATE(false, true);
    lf::cd_changer cd = create_temp_test_dir();
    write_text(test_path1, "abc");

    if (dest_exist) {
        fs::create_directories(test_path2.parent_path());
    }

    move_path(test_path1, test_path2);
    REQUIRE( !fs::exists(test_path1) );
    REQUIRE( read_text(test_path2) == "abc" );
}

TEST_CASE("path: move_path dir to dir", "[path]") {
    const bool dest_exist = GENERATE(false, true);
    lf::cd_changer cd = create_temp_test_dir();
    write_text(test_path1, "abc");

    if (dest_exist) {
        fs::create_directories(test_path2.parent_path());
    }

    move_path(test_path1.parent_path(), test_path2);
    REQUIRE( !fs::exists(test_path1) );
    REQUIRE( fs::is_directory(test_path2) );
    REQUIRE( read_text(test_path2 / test_path1.filename()) == "abc" );
}
