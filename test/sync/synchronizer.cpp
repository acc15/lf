#include <catch2/catch_test_macros.hpp>

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

const std::string test_name = "test.txt";
const std::string test_content = "abc\nxyz";

TEST_CASE("local_added", "[synchronizer]") {
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
    REQUIRE( fs::exists(sync.local / test_deep_path) );
}

TEST_CASE("local_deleted", "[synchronizer]") {
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

TEST_CASE("remote_deleted", "[synchronizer]") {
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

TEST_CASE("remote_added", "[synchronizer]") {
    // must copy file to local side and set state
    auto sync = make_sync();

    write_text(sync.remote / test_name, test_content);

    synchronizer s("test", sync);
    s.index = index_tree(sync_mode::UNSPECIFIED, {{ test_name, index_tree(sync_mode::SHALLOW) }});
    s.run();

    REQUIRE( read_text(sync.local / "test.txt") == test_content );
}

TEST_CASE("index_removed", "[synchronizer]") {
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
