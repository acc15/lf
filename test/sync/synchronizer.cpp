#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>
#include <optional>

#include "sync/synchronizer.hpp"
#include "test_util.hpp"

namespace fs = std::filesystem;
using namespace lf;

config::sync make_sync(std::optional<bool> local_to_remote = std::nullopt) {

    fs::path dest_test_dir = create_temp_test_dir(local_to_remote.has_value() 
        ? (local_to_remote ? "l2r" : "r2l") 
        : fs::path());

    fs::path local = dest_test_dir / "local";
    fs::path remote = dest_test_dir / "remote";

    fs::create_directories(local);
    fs::create_directories(remote);

    return {
        .name = "test",
        .local = local,
        .remote = remote,
        .state = fs::path(),
        .index = fs::path()
    };
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

const std::string test_name = "test.txt";
const std::string test_content = "abc\nxyz";

void write_test_file(const fs::path& path) {
    write_text(path, test_content);
}

TEST_CASE("synchronizer file added", "[synchronizer]") {

    const bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    config::sync sync = make_sync(local);
    lf::index index;
    lf::tracked_state state;

    const fs::path test_deep_path = fs::path("a") / test_name; 
    write_test_file((local ? sync.local : sync.remote) / test_deep_path);
    
    index.set(test_deep_path, sync_mode::SHALLOW);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( read_text((local ? sync.remote : sync.local) / test_deep_path ) == test_content );
    
    REQUIRE_FALSE( state.get(test_deep_path.parent_path()) );
    REQUIRE( state.get(test_deep_path) );

    REQUIRE( fs::last_write_time(sync.remote / test_deep_path) == fs::last_write_time(sync.local / test_deep_path) );
}

TEST_CASE("synchronizer file deleted", "[synchronizer]") {
    
    const bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    config::sync sync = make_sync(local);
    lf::index index;
    lf::tracked_state state;

    write_test_file((local ? sync.local : sync.remote) / test_name);

    index.set(test_name, sync_mode::SHALLOW);
    state.set(test_name, true);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( index.get(test_name) == sync_mode::SHALLOW );
    REQUIRE_FALSE( state.get(test_name) );
    REQUIRE_FALSE( fs::exists(sync.remote / test_name) );
    REQUIRE_FALSE( fs::exists(sync.local  / test_name) );

}

TEST_CASE("synchronizer file updated", "[synchronizer]") {

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    config::sync sync = make_sync(local);
    lf::index index;
    lf::tracked_state state;

    fs::path l = local ? sync.local : sync.remote;
    fs::path r = !local ? sync.local : sync.remote;

    write_text(l / test_name, test_content);
    write_text(r / test_name, "abc\n");

    auto change_t = plus50ms_last_write_time(l / test_name, r / test_name);

    index.set(test_name, sync_mode::SHALLOW);
    state.set(test_name, true);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( read_text(r / test_name) == test_content );
    REQUIRE( read_text(l / test_name) == test_content );
    REQUIRE( fs::last_write_time(r / test_name) == change_t );
    REQUIRE( fs::last_write_time(l / test_name) == change_t );
    REQUIRE( s.state.get(test_name) == true );

}

TEST_CASE("synchronizer index removed", "[synchronizer]") {
    auto sync = make_sync();
    lf::index index; 
    lf::tracked_state state;

    write_test_file(sync.local / test_name);
    write_test_file(sync.remote / test_name);

    state.set(test_name, true);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(sync.remote / test_name) );
    REQUIRE( fs::exists(sync.local / test_name) );
    REQUIRE( s.state.node(test_name) == nullptr );
}

TEST_CASE("synchronizer can update file timestamp", "[synchronizer]") {
    
    auto sync = make_sync();
    write_test_file(sync.local / test_name);
    write_test_file(sync.remote / test_name);
    plus50ms_last_write_time(sync.local / test_name, sync.remote / test_name);

}

TEST_CASE("synchronizer local shallow dir to remote", "[synchronizer]") {

    config::sync sync = make_sync();
    lf::index index;
    lf::tracked_state state;

    write_test_file(sync.local / "a" / "a.txt");
    write_test_file(sync.local / "a" / "b.txt");
    write_test_file(sync.local / "a" / "c.txt");

    index.set("a", sync_mode::SHALLOW);
    index.set(fs::path("a") / "a.txt", sync_mode::IGNORE);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE_FALSE( fs::exists(sync.remote / "a" / "a.txt") ); // ignored
    REQUIRE(read_text(sync.remote / "a" / "b.txt") == test_content );
    REQUIRE(read_text(sync.remote / "a" / "c.txt") == test_content );

    REQUIRE_FALSE( s.state.get(fs::path("a") / "a.txt") );
    REQUIRE( s.state.get(fs::path("a") / "b.txt") );
    REQUIRE( s.state.get(fs::path("a") / "c.txt") );

}

TEST_CASE("synchronizer update shallow dir file", "[synchronizer]") {

    config::sync sync = make_sync();
    lf::index index;
    lf::tracked_state state;

    const fs::path test_path = fs::path("a") / "a.txt";

    write_test_file(sync.local / test_path);
    write_text(sync.remote / test_path, "abc\n");

    plus50ms_last_write_time(sync.local / test_path, sync.remote / test_path);

    synchronizer s(sync, index, state);
    index.set(test_path.parent_path(), sync_mode::SHALLOW);
    state.set(test_path, true);
    s.run();

    REQUIRE( state.get(test_path) );
    REQUIRE( read_text(sync.remote / test_path) == test_content );

}

TEST_CASE("synchronizer delete shallow dir file", "[synchronizer]") {

    config::sync sync = make_sync();
    lf::index index;
    lf::tracked_state state;

    const fs::path a = fs::path("dir") / "a.txt";
    const fs::path b = fs::path("dir") / "b.txt";

    write_test_file(sync.local / a);
    write_test_file(sync.remote / a);
    write_test_file(sync.remote / b);

    index.set(a.parent_path(), sync_mode::SHALLOW);
    state.set(a, true);
    state.set(b, true);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( fs::exists(sync.remote / a ));
    REQUIRE_FALSE( fs::exists(sync.remote / b ));

    REQUIRE( index.get("dir") == sync_mode::SHALLOW );
    REQUIRE( state.get(a.parent_path()) );
    REQUIRE( state.get(a) );
    REQUIRE_FALSE( state.get(b) );

}

TEST_CASE("synchronizer file to dir", "[synchronizer]") {

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    config::sync sync = make_sync(local);
    lf::index index;
    lf::tracked_state state;

    const fs::path l = local ? sync.local : sync.remote;
    const fs::path r = local ? sync.remote : sync.local;

    write_test_file(l / test_name);
    write_test_file(r / test_name / "x.txt");

    plus50ms_last_write_time(l / test_name, r / test_name);

    index.set(test_name, sync_mode::SHALLOW);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( fs::status(r / test_name).type() == fs::file_type::regular );
    REQUIRE( read_text(r / test_name) == test_content );

}

TEST_CASE("synchronizer dir to file", "[synchronizer]") {

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    
    config::sync sync = make_sync(local);
    lf::index index;
    lf::tracked_state state;

    const fs::path l = local ? sync.local : sync.remote;
    const fs::path r = local ? sync.remote : sync.local;

    write_test_file(l / test_name / "x.txt");
    write_test_file(r / test_name);

    plus50ms_last_write_time(l / test_name, r / test_name);

    index.set(test_name, sync_mode::SHALLOW);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( fs::status(r / test_name).type() == fs::file_type::directory );
    REQUIRE( read_text(r / test_name / "x.txt") == test_content );

}

TEST_CASE("synchronizer nested file must override file", "[synchronizer]") {
    
    bool local = GENERATE(true, false);
    INFO( test_direction(local) );

    config::sync sync = make_sync(local);
    lf::index index;
    lf::tracked_state state;

    const fs::path l = local ? sync.local : sync.remote;
    const fs::path r = local ? sync.remote : sync.local;

    const fs::path test_path = fs::path("a") / test_name;
    write_test_file(l / test_path);
    write_test_file(r / test_path.parent_path());

    index.set(test_path, sync_mode::SHALLOW);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( fs::status(r / test_path.parent_path()).type() == fs::file_type::directory );
    REQUIRE( read_text(r / test_path) == test_content );

}

TEST_CASE("synchronizer avoid creating UNSPECIFIED directories", "[synchronizer]") {
    
    config::sync sync = make_sync();
    lf::index index;
    lf::tracked_state state;

    const fs::path l = sync.local;
    const fs::path r = sync.remote;

    const fs::path keep_dir = "a";
    const fs::path cleanup_dir = keep_dir / "b";
    const fs::path path = cleanup_dir / test_name;

    // file present in remote, but dir "test" removed in local
    fs::create_directory(l / keep_dir);
    write_test_file(r / path);

    state.set(path, true);

    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( state.empty() );
    REQUIRE( index.empty() );

    // both dirs is empty
    REQUIRE( fs::is_empty(l) ); 
    REQUIRE( fs::is_empty(r) );

}

TEST_CASE("synchronizer must keep local unsynced file", "[synchronizer]") {
    
    config::sync sync = make_sync();
    lf::index index;
    lf::tracked_state state;

    fs::path test_path = fs::path("a") / test_name;

    write_text(sync.local / test_path, test_content);
    
    state.set(test_path, true);
    synchronizer s(sync, index, state);
    s.run();

    REQUIRE( fs::exists(sync.local / test_path) );
    REQUIRE_FALSE( fs::exists(sync.remote / test_path) );
    REQUIRE_FALSE( state.get(test_path) );

}

TEST_CASE("synchronizer must sync permissions", "[synchronizer]") {

    config::sync sync = make_sync();
    lf::index index;
    lf::tracked_state state;

    const fs::path test_path = "a.txt";
    index.set(test_path, sync_mode::SHALLOW);

    write_text(sync.local / test_path, test_content);
    
    fs::perms perms = fs::perms::owner_all | fs::perms::group_all;

    fs::permissions(sync.local / test_path, perms);
    REQUIRE( fs::status(sync.local / test_path).permissions() == perms );

    synchronizer(sync, index, state).run();
    REQUIRE( fs::status(sync.remote / test_path).permissions() == perms );

    fs::permissions(sync.remote / test_path, fs::perms::owner_all);
    REQUIRE( fs::status(sync.remote / test_path).permissions() == fs::perms::owner_all );

    plus50ms_last_write_time(sync.remote / test_path, sync.local / test_path);

    synchronizer(sync, index, state).run();
    REQUIRE( fs::status(sync.local / test_path).permissions() == fs::perms::owner_all );

}
