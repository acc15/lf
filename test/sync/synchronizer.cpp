#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>
#include <optional>
#include <utility>

#include "sync/synchronizer.hpp"
#include "test_util.hpp"

namespace fs = std::filesystem;
using namespace lf;

config make_sync(std::optional<bool> local_to_remote = std::nullopt) {

    fs::path dest_test_dir = create_temp_test_dir(local_to_remote.has_value() 
        ? (local_to_remote ? "l2r" : "r2l") 
        : fs::path());

    fs::path local = dest_test_dir / "local";
    fs::path remote = dest_test_dir / "remote";

    fs::create_directories(local);
    fs::create_directories(remote);

    return {
        .local = local,
        .remote = remote,
        .state = fs::path(),
        .index = fs::path()
    };
}

const fs::path& pick_path(const config& config, bool local) {
    return local ? config.local : config.remote;
}

std::pair<fs::path, fs::path> make_paths(
    const config& config, 
    const fs::path& path = fs::path(), 
    bool swap = false
) {
    return std::make_pair(pick_path(config, !swap) / path, pick_path(config, swap) / path);
}

const char* test_direction(bool local) {
    return local ? "local to remote" : "remote to local";
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

    const bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    const config config = make_sync(local);
    const auto [l, r] = make_paths(config, test_deep_path, local);
    lf::index index;
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
    
    const bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    const config config = make_sync(local);
    const auto [l, r] = make_paths(config, test_path, local);

    lf::index index;
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

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    const config config = make_sync(local);
    const auto [l, r] = make_paths(config, test_path, local);

    lf::index index;
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
    lf::index index = { sync_mode::NONE, {{test_path.string(), {sync_mode::IGNORE}}}};  
    lf::tracked_state state = lf::state { false, {{test_path.string(), {true}}} };

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
    write_test_file(config.local / test_path);
    write_test_file(config.remote / test_path);
    plus50ms_last_write_time(config.local / test_path, config.remote / test_path);

}

TEST_CASE("synchronizer: local shallow dir to remote", "[synchronizer]") {

    config config = make_sync();
    lf::index index;
    lf::tracked_state state;

    write_test_file(config.local / "a" / "a.txt");
    write_test_file(config.local / "a" / "b.txt");
    write_test_file(config.local / "a" / "c.txt");

    index.set("a", sync_mode::SHALLOW);
    index.set(fs::path("a") / "a.txt", sync_mode::IGNORE);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(config.remote / "a" / "a.txt") ); // ignored
    REQUIRE(read_text(config.remote / "a" / "b.txt") == test_content );
    REQUIRE(read_text(config.remote / "a" / "c.txt") == test_content );

    REQUIRE_FALSE( s.state.get(fs::path("a") / "a.txt") );
    REQUIRE( s.state.get(fs::path("a") / "b.txt") );
    REQUIRE( s.state.get(fs::path("a") / "c.txt") );

}

TEST_CASE("synchronizer: update shallow dir file", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config, test_deep_path);
    lf::index index;
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
    lf::index index;
    lf::tracked_state state;

    const fs::path a = fs::path("dir") / "a.txt";
    const fs::path b = fs::path("dir") / "b.txt";

    write_test_file(config.local / a);
    write_test_file(config.remote / a);
    write_test_file(config.remote / b);

    index.set(a.parent_path(), sync_mode::SHALLOW);
    state.set(a, true);
    state.set(b, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::exists(config.remote / a ));
    REQUIRE_FALSE( fs::exists(config.remote / b ));

    REQUIRE( index.get("dir") == sync_mode::SHALLOW );
    REQUIRE( state.get(a.parent_path()) );
    REQUIRE( state.get(a) );
    REQUIRE_FALSE( state.get(b) );

}

TEST_CASE("synchronizer: file to dir", "[synchronizer]") {

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    config config = make_sync(local);
    const auto [l, r] = make_paths(config, test_path, local);

    lf::index index;
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

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    const config config = make_sync(local);
    const auto [l, r] = make_paths(config, test_path, local);

    lf::index index;
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
    
    bool local = GENERATE(true, false);
    INFO( test_direction(local) );

    const config config = make_sync(local);
    const auto [l, r] = make_paths(config, test_deep_path, local);

    lf::index index;
    lf::tracked_state state;

    write_test_file(l);
    write_test_file(r.parent_path());

    index.set(test_deep_path, sync_mode::SHALLOW);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( fs::status(r.parent_path()).type() == fs::file_type::directory );
    REQUIRE( read_text(r) == test_content );

}

TEST_CASE("synchronizer: must delete empty local and remote directories and remote files", "[synchronizer]") {
    
    const config config = make_sync();
    lf::index index;
    lf::tracked_state state;

    const auto [l, r] = make_paths(config);

    const fs::path keep_dir = "a";
    const fs::path cleanup_dir = keep_dir / "b";
    const fs::path path = cleanup_dir / test_path;

    // file present in remote, but dir "test" removed in local
    fs::create_directory(l / keep_dir);
    write_test_file(r / path);

    state.set(path, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE( state.empty() );
    REQUIRE( index.empty() );

    REQUIRE( fs::is_empty(l) ); 
    REQUIRE( fs::is_empty(r) );

}

TEST_CASE("synchronizer: must keep local unsynced file", "[synchronizer]") {
    
    const config config = make_sync();
    const auto [l, r] = make_paths(config, test_deep_path);

    lf::index index;
    lf::tracked_state state;
    index.set(test_deep_path.parent_path(), sync_mode::RECURSIVE);
    index.set(test_deep_path, sync_mode::NONE);

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

    lf::index index;
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

TEST_CASE("synchronizer: not synced, but local and remote are same", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config, test_path);

    lf::index index = { sync_mode::NONE, { { test_path.string(), { sync_mode::SHALLOW } }} };
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

TEST_CASE("synchronizer: remote file deleted, index deleted, local synced - must delete local", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config, test_path);

    write_test_file(l);

    lf::index index;
    lf::tracked_state state;
    state.set(test_path, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(l) );
    REQUIRE_FALSE( fs::exists(r) );
    REQUIRE_FALSE( state.get(test_path) );

}

TEST_CASE("synchronizer: remote dir, local synced file - must delete local file", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config, test_path);

    fs::create_directories(r);
    write_test_file(l);

    lf::index index;
    lf::tracked_state state;
    state.set(test_path, true);

    synchronizer s(config, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(l) );
    REQUIRE_FALSE( fs::exists(r) );
    REQUIRE_FALSE( state.get(test_path) );

}

TEST_CASE("synchronizer: recursive, must keep local ignored dir", "[synchronizer]") {

    config config = make_sync();
    const auto [l, r] = make_paths(config);

    const fs::path root_path = "a";
    const fs::path synced_path = root_path / "d.txt";
    const fs::path ignored_path = root_path / "b";
    const fs::path ignored_file = ignored_path /  "c.txt";

    lf::index index;
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