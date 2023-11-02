#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>
#include <optional>
#include <utility>

#include "sync/synchronizer.hpp"
#include "test_util.hpp"

namespace fs = std::filesystem;
using namespace lf;

config make_sync(std::optional<bool> left_to_right = std::nullopt) {
    return make_test_config(left_to_right.has_value() 
        ? (left_to_right ? "l2r" : "r2l") 
        : fs::path());
}

const fs::path& pick_path(const config& config, bool is_left) {
    return is_left ? config.left : config.right;
}

std::pair<fs::path, fs::path> make_paths(
    const config& config, 
    const fs::path& path = fs::path(), 
    bool swap = false
) {
    return std::make_pair(pick_path(config, !swap) / path, pick_path(config, swap) / path);
}

const char* test_direction(bool is_left) {
    return is_left ? "left to right" : "right to left";
}

fs::file_time_type plus50ms_last_write_time(const std::filesystem::path& dst, const std::filesystem::path& src) {
    fs::file_time_type old_t = fs::last_write_time(src); 
    fs::file_time_type new_t = old_t + std::chrono::milliseconds(50);
    fs::last_write_time(dst, new_t);
    
    auto actual_t = fs::last_write_time(dst); 
    REQUIRE( actual_t > old_t );
    REQUIRE( actual_t == new_t );
    return actual_t;
}

const fs::path test_path = "test.txt";
const fs::path test_deep_path = fs::path("a") / test_path;
const std::string test_content = "abc\nxyz";

void write_test_file(const fs::path& path) {
    write_text(path, test_content);
}

TEST_CASE("synchronizer: file added", "[synchronizer]") {

    const bool is_left = GENERATE(true, false);
    INFO( test_direction(is_left) );
    
    const config config = make_sync(is_left);
    const auto [l, r] = make_paths(config, test_deep_path, is_left);
    lf::tracked_index index;
    lf::tracked_state state;
    index.set(test_deep_path, sync_mode::SHALLOW);

    write_test_file(l);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( read_text(r) == test_content );
    
    REQUIRE_FALSE( state.get(test_deep_path.parent_path()) );
    REQUIRE( state.get(test_deep_path) );

    REQUIRE( fs::last_write_time(l) == fs::last_write_time(r) );
}

TEST_CASE("synchronizer: file deleted", "[synchronizer]") {
    
    const bool is_left = GENERATE(true, false);
    INFO( test_direction(is_left) );
    
    const config config = make_sync(is_left);
    const auto [l, r] = make_paths(config, test_path, is_left);

    lf::tracked_index index;
    lf::tracked_state state;

    write_test_file(l);

    index.set(test_path, sync_mode::SHALLOW);
    state.set(test_path, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( index.get(test_path) == sync_mode::SHALLOW );
    REQUIRE_FALSE( state.get(test_path) );
    REQUIRE_FALSE( fs::exists(r) );
    REQUIRE_FALSE( fs::exists(l) );

}

TEST_CASE("synchronizer: file updated", "[synchronizer]") {

    bool is_left = GENERATE(true, false);
    INFO( test_direction(is_left) );
    
    const config config = make_sync(is_left);
    const auto [l, r] = make_paths(config, test_path, is_left);

    lf::tracked_index index;
    lf::tracked_state state;

    write_text(l, test_content);
    write_text(r, "abc\n");

    auto change_t = plus50ms_last_write_time(l, r);

    index.set(test_path, sync_mode::SHALLOW);
    state.set(test_path, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( read_text(r) == test_content );
    REQUIRE( read_text(l) == test_content );
    REQUIRE( fs::last_write_time(r) == change_t );
    REQUIRE( fs::last_write_time(l) == change_t );
    REQUIRE( s.state.get(test_path) == true );

}

TEST_CASE("synchronizer: file removed from index (ignored)", "[synchronizer]") {
    const auto config = make_sync();
    const auto [l, r] = make_paths(config, test_path);
    
    lf::tracked_index index; 
    index.set(test_path, sync_mode::IGNORE);
    
    lf::tracked_state state;
    state.set(test_path, true);

    write_test_file(l);
    write_test_file(r);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(r) );
    REQUIRE( fs::exists(l) );
    REQUIRE( s.state.node(test_path) == nullptr );
}

TEST_CASE("synchronizer: can update file timestamp", "[synchronizer]") {
    
    const auto config = make_sync();
    write_test_file(config.left / test_path);
    write_test_file(config.right / test_path);
    plus50ms_last_write_time(config.left / test_path, config.right / test_path);

}

TEST_CASE("synchronizer: left shallow dir to right", "[synchronizer]") {

    config config = make_sync();
    
    lf::tracked_index index;
    index.set("a", sync_mode::SHALLOW);
    index.set(fs::path("a") / "a.txt", sync_mode::IGNORE);

    lf::tracked_state state;

    write_test_file(config.left / "a" / "a.txt");
    write_test_file(config.left / "a" / "b.txt");
    write_test_file(config.left / "a" / "c.txt");

    synchronizer s(config, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(config.right / "a" / "a.txt") ); // ignored
    REQUIRE(read_text(config.right / "a" / "b.txt") == test_content );
    REQUIRE(read_text(config.right / "a" / "c.txt") == test_content );

    REQUIRE_FALSE( s.state.get(fs::path("a") / "a.txt") );
    REQUIRE( s.state.get(fs::path("a") / "b.txt") );
    REQUIRE( s.state.get(fs::path("a") / "c.txt") );

}

TEST_CASE("synchronizer: update shallow dir file", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config, test_deep_path);
    
    lf::tracked_index index;
    lf::tracked_state state;

    write_test_file(l);
    write_text(r, "abc\n");

    plus50ms_last_write_time(l, r);

    synchronizer s(config, index, state);
    index.set(test_deep_path.parent_path(), sync_mode::SHALLOW);
    state.set(test_deep_path, true);
    s.run();

    REQUIRE( state.get(test_deep_path) );
    REQUIRE( read_text(r) == test_content );

}

TEST_CASE("synchronizer: delete shallow dir file", "[synchronizer]") {

    config config = make_sync();
    lf::tracked_index index;
    lf::tracked_state state;

    const fs::path a = fs::path("dir") / "a.txt";
    const fs::path b = fs::path("dir") / "b.txt";

    write_test_file(config.left / a);
    write_test_file(config.right / a);
    write_test_file(config.right / b);

    index.set(a.parent_path(), sync_mode::SHALLOW);
    state.set(a, true);
    state.set(b, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::exists(config.right / a ));
    REQUIRE_FALSE( fs::exists(config.right / b ));

    REQUIRE( index.get("dir") == sync_mode::SHALLOW );
    REQUIRE( state.get(a.parent_path()) );
    REQUIRE( state.get(a) );
    REQUIRE_FALSE( state.get(b) );

}

TEST_CASE("synchronizer: file to dir", "[synchronizer]") {

    bool is_left = GENERATE(true, false);
    INFO( test_direction(is_left) );
    
    const config config = make_sync(is_left);
    const auto [l, r] = make_paths(config, test_path, is_left);

    lf::tracked_index index;
    lf::tracked_state state;

    write_test_file(l);
    write_test_file(r / "x.txt");

    plus50ms_last_write_time(l, r);

    index.set(test_path, sync_mode::SHALLOW);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::status(r).type() == fs::file_type::regular );
    REQUIRE( read_text(r) == test_content );

}

TEST_CASE("synchronizer: dir to file", "[synchronizer]") {

    bool is_left = GENERATE(true, false);
    INFO( test_direction(is_left) );
    
    const config config = make_sync(is_left);
    const auto [l, r] = make_paths(config, test_path, is_left);

    lf::tracked_index index;
    lf::tracked_state state;

    write_test_file(l / "x.txt");
    write_test_file(r);

    plus50ms_last_write_time(l, r);

    index.set(test_path, sync_mode::SHALLOW);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::status(r).type() == fs::file_type::directory );
    REQUIRE( read_text(r / "x.txt") == test_content );

}

TEST_CASE("synchronizer: nested file must override file", "[synchronizer]") {
    
    bool is_left = GENERATE(true, false);
    INFO( test_direction(is_left) );

    const config config = make_sync(is_left);
    const auto [l, r] = make_paths(config, test_deep_path, is_left);

    lf::tracked_index index;
    lf::tracked_state state;

    write_test_file(l);
    write_test_file(r.parent_path());

    index.set(test_deep_path, sync_mode::SHALLOW);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::status(r.parent_path()).type() == fs::file_type::directory );
    REQUIRE( read_text(r) == test_content );

}

TEST_CASE("synchronizer: must delete empty left and right directories and right files", "[synchronizer]") {
    
    const config config = make_sync();
    lf::tracked_index index;
    lf::tracked_state state;

    const auto [l, r] = make_paths(config);

    const fs::path keep_dir = "a";
    const fs::path cleanup_dir = keep_dir / "b";
    const fs::path path = cleanup_dir / test_path;

    // file present in right, but dir "test" removed in left
    fs::create_directory(l / keep_dir);
    write_test_file(r / path);

    state.set(path, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( state.root.entries.empty() );
    REQUIRE( index.root.entries.empty() );

    REQUIRE( fs::is_empty(l) ); 
    REQUIRE( fs::is_empty(r) );

}

TEST_CASE("synchronizer: must keep left IGNORE file", "[synchronizer]") {
    
    const config config = make_sync();
    const auto [l, r] = make_paths(config, test_deep_path);

    lf::tracked_index index;
    lf::tracked_state state;
    index.set(test_deep_path.parent_path(), sync_mode::RECURSIVE);
    index.set(test_deep_path, sync_mode::IGNORE);

    write_text(l, test_content);
    fs::create_directories(r);
    
    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::exists(l) );
    REQUIRE_FALSE( fs::exists(r) );
    REQUIRE_FALSE( state.get(test_deep_path) );

}


TEST_CASE("synchronizer: must sync permissions", "[synchronizer]") {

    const fs::path test_path = "a.txt";
    const config config = make_sync();
    const auto [l, r] = make_paths(config, test_path);

    lf::tracked_index index;
    lf::tracked_state state;

    index.set(test_path, sync_mode::SHALLOW);

    write_text(l, test_content);
    
    fs::perms perms = fs::perms::owner_all | fs::perms::group_all;

    fs::permissions(l, perms);
    REQUIRE( fs::status(l).permissions() == perms );

    synchronizer(config, index, state).run();
    REQUIRE( fs::status(r).permissions() == perms );

    fs::permissions(r, fs::perms::owner_all);
    REQUIRE( fs::status(r).permissions() == fs::perms::owner_all );

    plus50ms_last_write_time(r, l);

    synchronizer(config, index, state).run();
    REQUIRE( fs::status(l).permissions() == fs::perms::owner_all );

}

TEST_CASE("synchronizer: not synced, but left and right are same", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config, test_path);

    lf::tracked_index index;
    index.set(test_path, sync_mode::SHALLOW);
    
    lf::tracked_state state;

    write_text(l, test_content);
    fs::copy_file(l, r);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::exists(l) );
    REQUIRE( fs::exists(r) );
    REQUIRE( fs::last_write_time(l) == fs::last_write_time(r) );
    REQUIRE( state.get(test_path) );

}

TEST_CASE("synchronizer: right file deleted, index deleted, left synced - must delete left", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config, test_path);

    write_test_file(l);

    lf::tracked_index index;
    lf::tracked_state state;
    state.set(test_path, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(l) );
    REQUIRE_FALSE( fs::exists(r) );
    REQUIRE_FALSE( state.get(test_path) );

}

TEST_CASE("synchronizer: right dir, left synced file - must delete left file", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config, test_path);

    fs::create_directories(r);
    write_test_file(l);

    lf::tracked_index index;
    lf::tracked_state state;
    state.set(test_path, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(l) );
    REQUIRE_FALSE( fs::exists(r) );
    REQUIRE_FALSE( state.get(test_path) );

}

TEST_CASE("synchronizer: recursive, must keep left ignored dir", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config);

    const fs::path root_path = "a";
    const fs::path synced_path = root_path / "d.txt";
    const fs::path ignored_path = root_path / "b";
    const fs::path ignored_file = ignored_path /  "c.txt";

    lf::tracked_index index;
    index.set(root_path, sync_mode::RECURSIVE);
    index.set(ignored_path, sync_mode::IGNORE);

    lf::tracked_state state;
    state.set(root_path, true);
    state.set(synced_path, true);
    state.set(ignored_path, true);
    state.set(ignored_file, true);

    write_test_file(l / ignored_file);
    write_test_file(l / synced_path);
    fs::create_directories(r / root_path);
    fs::copy_file(l / synced_path, r / synced_path);

    synchronizer s(config, index, state);
    s.run();
    
    REQUIRE( fs::exists(l / ignored_file) );
    REQUIRE( state.get(synced_path) );
    REQUIRE_FALSE( state.get(ignored_file) );
    REQUIRE_FALSE( state.get(ignored_path) );

}

TEST_CASE("synchronizer: must inherit IGNORE mode if NONE in index", "[synchronizer]") {

    const auto config = make_sync();
    const auto [l, r] = make_paths(config);

    const fs::path root_dir = "a";
    const fs::path intermediate_dir = root_dir / "b";
    const fs::path shallow_sync_file = intermediate_dir / "test.txt";

    lf::tracked_index index;
    index.set(root_dir, sync_mode::IGNORE);
    index.set(shallow_sync_file, sync_mode::SHALLOW);

    fs::create_directories(l / intermediate_dir);

    lf::tracked_state state;
    state.set(shallow_sync_file, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::exists(l / intermediate_dir) );
    REQUIRE_FALSE( state.get(shallow_sync_file) );
    REQUIRE_FALSE( state.get(intermediate_dir) );
    REQUIRE_FALSE( state.get(root_dir) );

}

TEST_CASE("synchronizer: must inherit RECURSIVE mode if NONE in index", "[synchronizer]") {

    const auto config = make_sync();
    const auto [l, r] = make_paths(config);

    const fs::path root_dir = "a";
    const fs::path intermediate_dir = root_dir / "b";
    const fs::path ignore_dir = intermediate_dir / "c";
    const fs::path recursive_sync_file = intermediate_dir / "test.txt";

    lf::tracked_index index;
    index.set(root_dir, sync_mode::RECURSIVE);
    index.set(ignore_dir, sync_mode::IGNORE);

    write_test_file(l / recursive_sync_file);

    lf::tracked_state state;
    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::exists(l / recursive_sync_file) );
    REQUIRE( fs::exists(r / recursive_sync_file) );
    REQUIRE( read_text(l / recursive_sync_file) == read_text(r / recursive_sync_file) );
    REQUIRE( state.get(recursive_sync_file) );
    REQUIRE( state.get(intermediate_dir) );
    REQUIRE( state.get(root_dir) );

}