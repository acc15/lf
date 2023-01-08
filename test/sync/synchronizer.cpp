#include <catch2/catch_test_macros.hpp>

#include <thread>

#include "sync/synchronizer.hpp"
#include "test_util.hpp"

namespace fs = std::filesystem;
using namespace lf;

config::sync make_sync() {
    std::string name = Catch::getResultCapture().getCurrentTestName();
    fs::path dest_test_dir = fs::temp_directory_path() / "lf_test" / "synchronizer" / name;

    fs::remove_all(dest_test_dir);
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

TEST_CASE("local added", "[synchronizer]") {
    // must copy to remote and set state
    auto sync = make_sync();

    const fs::path test_deep_path = fs::path("a") / test_name; 
    write_text(sync.local / test_deep_path, test_content);
    
    synchronizer s("test", sync);
    s.index = index_tree(sync_mode::UNSPECIFIED, { 
        { "a", index_tree(sync_mode::UNSPECIFIED, { { test_name, index_tree(sync_mode::SHALLOW) } }) }
    }); 
    s.run();

    REQUIRE( s.state.node(test_deep_path) != nullptr );
    REQUIRE( read_text(sync.remote / test_deep_path ) == test_content );
    REQUIRE( fs::last_write_time(sync.remote / test_deep_path) == fs::last_write_time(sync.local / test_deep_path) );
    REQUIRE( fs::exists(sync.local / test_deep_path) );
}

TEST_CASE("local deleted", "[synchronizer]") {
    // must remove file from remote, remove it from index and reset sync
    auto sync = make_sync();

    write_text(sync.remote / test_name, test_content);

    synchronizer s("test", sync);
    s.index = index_tree(sync_mode::UNSPECIFIED, { {test_name, index_tree(sync_mode::SHALLOW) } });
    s.state = state_tree(false, { {test_name, state_tree(true) } });
    s.run();

    // removed from state and index
    REQUIRE_FALSE( s.state.node(test_name) == nullptr ); 
    REQUIRE_FALSE( s.index.node(test_name) == nullptr );

    REQUIRE_FALSE( fs::exists(sync.remote / test_name) );
    REQUIRE_FALSE( fs::exists(sync.local  / test_name) );
}

TEST_CASE("remote deleted", "[synchronizer]") {
    // must remove file from local and reset state
    auto sync = make_sync();

    write_text(sync.local / test_name, test_content);
    
    synchronizer s("test", sync);
    s.index = index_tree();
    s.state = state_tree(false, { {test_name, state_tree(true) } });
    s.run();

    REQUIRE_FALSE( fs::exists(sync.remote / test_name) );
    REQUIRE_FALSE( fs::exists(sync.local / test_name) );
}

TEST_CASE("remote added", "[synchronizer]") {
    // must copy file to local side and set state
    auto sync = make_sync();

    write_text(sync.remote / test_name, test_content);

    synchronizer s("test", sync);
    s.index = index_tree(sync_mode::UNSPECIFIED, {{ test_name, index_tree(sync_mode::SHALLOW) }});
    s.run();

    REQUIRE( read_text(sync.local / "test.txt") == test_content );
}

TEST_CASE("index removed", "[synchronizer]") {
    // must remove from remote side, but keep local and reset state 
    auto sync = make_sync();

    write_text(sync.local / test_name, test_content);
    write_text(sync.remote / test_name, test_content);

    synchronizer s("test", sync);
    s.run();

    REQUIRE_FALSE( fs::exists(sync.remote / test_name) );
    REQUIRE( fs::exists(sync.local / test_name) );
    REQUIRE( s.state.node(test_name) == nullptr );
}

TEST_CASE("can update file timestamp", "[synchronizer]") {
    auto sync = make_sync();
    write_text(sync.local / test_name, "abc\n");
    write_text(sync.remote / test_name, "abc\n");
    plus50ms_last_write_time(sync.local / test_name, sync.remote / test_name);
}

TEST_CASE("update local from remote", "[synchronizer]") {

    auto sync = make_sync();

    write_text(sync.local / test_name, "abc\n");
    write_text(sync.remote / test_name, test_content);

    auto change_t = plus50ms_last_write_time(sync.local / test_name, sync.remote / test_name);

    synchronizer s("test", sync);
    s.index = index_tree(sync_mode::UNSPECIFIED, { {test_name, index_tree(sync_mode::SHALLOW) } });
    s.run();

    REQUIRE( read_text(sync.remote / test_name) == test_content );
    REQUIRE( read_text(sync.local / test_name) == test_content );
    REQUIRE( fs::last_write_time(sync.remote / test_name) == change_t );
    REQUIRE( fs::last_write_time(sync.local / test_name) == change_t );
    REQUIRE( s.state.get(test_name) == true );

}

TEST_CASE("update remote from local", "[synchronizer]") {

    auto sync = make_sync();

    write_text(sync.local / test_name, test_content);
    write_text(sync.remote / test_name, "abc\n");

    auto change_t = plus50ms_last_write_time(sync.local / test_name, sync.remote / test_name);

    synchronizer s("test", sync);
    s.index = index_tree(sync_mode::UNSPECIFIED, { {test_name, index_tree(sync_mode::SHALLOW) } });
    s.run();

    REQUIRE( read_text(sync.local / test_name) == test_content );
    REQUIRE( read_text(sync.remote / test_name) == test_content );
    REQUIRE( fs::last_write_time(sync.local / test_name) == change_t );
    REQUIRE( fs::last_write_time(sync.remote / test_name) == change_t );
    REQUIRE( s.state.get(test_name) == true );

}