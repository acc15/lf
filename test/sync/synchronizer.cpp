#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>

#include "io/format_stream.hpp"
#include "sync/synchronizer.hpp"
#include "test_util.hpp"

namespace fs = std::filesystem;
using namespace lf;

config::sync make_sync(const std::string& suffix = "") {
    std::string name = Catch::getResultCapture().getCurrentTestName();
    if (!suffix.empty()) {
        name += " " + suffix;
    }

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

TEST_CASE("local file added", "[synchronizer]") {
    // must copy to remote and set state
    auto sync = make_sync();

    const fs::path test_deep_path = fs::path("a") / test_name; 
    write_text(sync.local / test_deep_path, test_content);
    
    synchronizer s("test", sync, false);
    s.index = lf::index(sync_mode::UNSPECIFIED, { 
        { "a", lf::index(sync_mode::UNSPECIFIED, { { test_name, lf::index(sync_mode::SHALLOW) } }) }
    }); 
    s.run();

    REQUIRE( s.state.node(test_deep_path) != nullptr );
    REQUIRE( read_text(sync.remote / test_deep_path ) == test_content );
    REQUIRE( fs::last_write_time(sync.remote / test_deep_path) == fs::last_write_time(sync.local / test_deep_path) );
    REQUIRE( fs::exists(sync.local / test_deep_path) );

}

TEST_CASE("local file deleted", "[synchronizer]") {
    // must remove file from remote, remove it from index and reset sync
    auto sync = make_sync();

    write_text(sync.remote / test_name, test_content);

    synchronizer s("test", sync, false);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {test_name, lf::index(sync_mode::SHALLOW) } });
    s.state = lf::state(false, { {test_name, lf::state(true) } });
    s.run();

    REQUIRE( s.index.node(test_name) != nullptr );

    // removed from state
    REQUIRE( s.state.node(test_name) == nullptr ); 

    REQUIRE_FALSE( fs::exists(sync.remote / test_name) );
    REQUIRE_FALSE( fs::exists(sync.local  / test_name) );
}

TEST_CASE("remote file deleted", "[synchronizer]") {
    // must remove file from local and reset state
    auto sync = make_sync();

    write_text(sync.local / test_name, test_content);
    
    synchronizer s("test", sync, false);
    // index is empty due to file already removed from index (see test above)
    s.state = lf::state(false, { {test_name, lf::state(true) } });
    s.run();

    REQUIRE_FALSE( fs::exists(sync.remote / test_name) );
    REQUIRE_FALSE( fs::exists(sync.local / test_name) );
    REQUIRE_FALSE( s.state.get(test_name) );

}

TEST_CASE("remote added", "[synchronizer]") {
    // must copy file to local side and set state
    auto sync = make_sync();

    write_text(sync.remote / test_name, test_content);

    synchronizer s("test", sync, false);
    s.index = lf::index(sync_mode::UNSPECIFIED, {{ test_name, lf::index(sync_mode::SHALLOW) }});
    s.run();

    REQUIRE( read_text(sync.local / "test.txt") == test_content );
}

TEST_CASE("index removed", "[synchronizer]") {
    // must remove from remote side, but keep local and reset state 
    auto sync = make_sync();

    write_text(sync.local / test_name, test_content);
    write_text(sync.remote / test_name, test_content);

    synchronizer s("test", sync, false);
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

TEST_CASE("update file", "[synchronizer]") {

    bool local_or_remote = GENERATE(true, false);

    std::string name = local_or_remote ? "local to remote" : "remote to local";
    INFO(name);

    auto sync = make_sync(" " + name);

    fs::path l = local_or_remote ? sync.local : sync.remote;
    fs::path r = !local_or_remote ? sync.local : sync.remote;

    write_text(l / test_name, "abc\n");
    write_text(r / test_name, test_content);

    auto change_t = plus50ms_last_write_time(l / test_name, r / test_name);

    synchronizer s("test", sync, false);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {test_name, lf::index(sync_mode::SHALLOW) } });
    s.run();

    REQUIRE( read_text(r / test_name) == test_content );
    REQUIRE( read_text(l / test_name) == test_content );
    REQUIRE( fs::last_write_time(r / test_name) == change_t );
    REQUIRE( fs::last_write_time(l / test_name) == change_t );
    REQUIRE( s.state.get(test_name) == true );

}

TEST_CASE("file/dir ovewrite", "[synchronizer]") {
    
    bool local_or_remote = GENERATE(true, false);
    bool file_or_dir = GENERATE(true, false);

    std::string name = lf::format_stream()
        << (local_or_remote ? "local" : "remote") << " "
        << (file_or_dir ? "file" : "dir") << " to "
        << (!local_or_remote ? "local" : "remote") << " "
        << (!file_or_dir ? "file" : "dir");

    INFO(" " + name);

    auto sync = make_sync(name);

    write_text((local_or_remote ^ file_or_dir ? sync.remote : sync.local) / test_name, test_content);
    write_text((local_or_remote ^ file_or_dir ? sync.local : sync.remote) / test_name / "x.txt", test_content);

    plus50ms_last_write_time(
        (local_or_remote ? sync.local : sync.remote) / test_name, 
        (!local_or_remote ? sync.local : sync.remote) / test_name
    );

    synchronizer s("test", sync, false);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {test_name, lf::index(sync_mode::SHALLOW) } });
    s.run();

    fs::file_type expected_type = file_or_dir ? fs::file_type::regular : fs::file_type::directory;
    fs::path target_path = (local_or_remote ? sync.remote : sync.local) / test_name;
    REQUIRE( fs::status(target_path).type() == expected_type );
    
    fs::path check_path = target_path / test_name;
    if (!file_or_dir) {
        check_path /= "x.txt";
    } 

    REQUIRE( read_text(check_path) == test_content );

}

TEST_CASE("local shallow dir to remote", "[synchronizer]") {

    auto sync = make_sync();

    write_text(sync.local / "a" / "a.txt", test_content);
    write_text(sync.local / "a" / "b.txt", test_content);
    write_text(sync.local / "a" / "c.txt", test_content);

    synchronizer s("test", sync, false);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {"a", lf::index(sync_mode::SHALLOW, { { "a.txt", lf::index(sync_mode::IGNORE) } }) } });
    s.run();

    REQUIRE_FALSE( fs::exists(sync.remote / "a" / "b.txt") ); // ignored
    REQUIRE(read_text(sync.remote / "a" / "b.txt") == test_content );
    REQUIRE(read_text(sync.remote / "a" / "c.txt") == test_content );

    REQUIRE_FALSE( s.state.get(fs::path("a") / "a.txt") );
    REQUIRE( s.state.get(fs::path("a") / "b.txt") );
    REQUIRE( s.state.get(fs::path("a") / "c.txt") );

}