#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <string>

#include "sync/synchronizer.hpp"
#include "test_util.hpp"

namespace fs = std::filesystem;
using namespace lf;

config::sync make_sync(bool local_to_remote) {
    std::string name = Catch::getResultCapture().getCurrentTestName() + " " + (local_to_remote ? "L2R" : "R2L");

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

TEST_CASE("file added", "[synchronizer]") {

    const bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    auto sync = make_sync(local);

    const fs::path test_deep_path = fs::path("a") / test_name; 
    write_text((local ? sync.local : sync.remote) / test_deep_path, test_content);
    
    synchronizer s(sync);
    s.index = lf::index(sync_mode::UNSPECIFIED, { { "a", lf::index(sync_mode::UNSPECIFIED, { { test_name, lf::index(sync_mode::SHALLOW) } }) } });
    s.run();

    REQUIRE( read_text((local ? sync.remote : sync.local) / test_deep_path ) == test_content );
    
    REQUIRE_FALSE( s.state.get(test_deep_path.parent_path()) );
    REQUIRE( s.state.get(test_deep_path) );

    REQUIRE( fs::last_write_time(sync.remote / test_deep_path) == fs::last_write_time(sync.local / test_deep_path) );
}

TEST_CASE("file deleted", "[synchronizer]") {
    
    const bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    auto sync = make_sync(local);

    write_text((local ? sync.local : sync.remote) / test_name, test_content);
    
    synchronizer s(sync);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {test_name, lf::index(sync_mode::SHALLOW) } }); 
    s.state = lf::state(false, { {test_name, lf::state(true) } });
    s.run();

    REQUIRE( s.index.node(test_name) == nullptr );
    REQUIRE( s.state.node(test_name) == nullptr ); 
    REQUIRE_FALSE( s.state.get() );
    REQUIRE_FALSE( fs::exists(sync.remote / test_name) );
    REQUIRE_FALSE( fs::exists(sync.local  / test_name) );

}

TEST_CASE("file updated", "[synchronizer]") {

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    auto sync = make_sync(local);

    fs::path l = local ? sync.local : sync.remote;
    fs::path r = !local ? sync.local : sync.remote;

    write_text(l / test_name, test_content);
    write_text(r / test_name, "abc\n");

    auto change_t = plus50ms_last_write_time(l / test_name, r / test_name);

    synchronizer s(sync);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {test_name, lf::index(sync_mode::SHALLOW) } });
    s.state = lf::state(false, { { test_name, lf::state(true) }});
    s.run();

    REQUIRE( read_text(r / test_name) == test_content );
    REQUIRE( read_text(l / test_name) == test_content );
    REQUIRE( fs::last_write_time(r / test_name) == change_t );
    REQUIRE( fs::last_write_time(l / test_name) == change_t );
    REQUIRE_FALSE( s.state.get() );
    REQUIRE( s.state.get(test_name) == true );

}

TEST_CASE("index removed", "[synchronizer]") {
    auto sync = make_sync(true);

    write_text(sync.local / test_name, test_content);
    write_text(sync.remote / test_name, test_content);

    synchronizer s(sync);
    s.state = lf::state(false, { { test_name, { true }}});
    s.run();

    REQUIRE( fs::exists(sync.remote / test_name) );
    REQUIRE( fs::exists(sync.local / test_name) );
    REQUIRE( s.state.node(test_name) == nullptr );
}

TEST_CASE("can update file timestamp", "[synchronizer]") {
    
    auto sync = make_sync(true);
    write_text(sync.local / test_name, "abc\n");
    write_text(sync.remote / test_name, "abc\n");
    plus50ms_last_write_time(sync.local / test_name, sync.remote / test_name);

}

TEST_CASE("local shallow dir to remote", "[synchronizer]") {

    auto sync = make_sync(true);

    write_text(sync.local / "a" / "a.txt", test_content);
    write_text(sync.local / "a" / "b.txt", test_content);
    write_text(sync.local / "a" / "c.txt", test_content);

    synchronizer s(sync);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {"a", lf::index(sync_mode::SHALLOW, { { "a.txt", lf::index(sync_mode::IGNORE) } }) } });
    s.run();

    REQUIRE_FALSE( fs::exists(sync.remote / "a" / "a.txt") ); // ignored
    REQUIRE(read_text(sync.remote / "a" / "b.txt") == test_content );
    REQUIRE(read_text(sync.remote / "a" / "c.txt") == test_content );

    REQUIRE_FALSE( s.state.get(fs::path("a") / "a.txt") );
    REQUIRE( s.state.get(fs::path("a") / "b.txt") );
    REQUIRE( s.state.get(fs::path("a") / "c.txt") );

}

TEST_CASE("update shallow dir file", "[synchronizer]") {

    auto sync = make_sync(true);

    const fs::path test_path = fs::path("a") / "a.txt";

    write_text(sync.local / test_path, test_content);
    write_text(sync.remote / test_path, "abc\n");

    plus50ms_last_write_time(sync.local / test_path, sync.remote / test_path);

    synchronizer s(sync);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {"a", lf::index(sync_mode::SHALLOW) } });
    s.state = lf::state(false, {{ "a", lf::state(true, {{"a.txt", lf::state(true)}})}});
    s.run();

    REQUIRE( s.state.get(test_path) );
    REQUIRE(read_text(sync.remote / test_path) == test_content);

}

TEST_CASE("delete shallow dir file", "[synchronizer]") {

    auto sync = make_sync(true);

    const fs::path a = fs::path("dir") / "a.txt";
    const fs::path b = fs::path("dir") / "b.txt";

    write_text(sync.local / a, test_content);
    write_text(sync.remote / a, test_content);
    write_text(sync.remote / b, test_content);

    synchronizer s(sync);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {"dir", lf::index(sync_mode::SHALLOW)} });
    s.state = lf::state(false, {{"dir", lf::state(true, {
        {"a.txt", lf::state(true)},
        {"b.txt", lf::state(true)}
    }) }});
    s.run();

    REQUIRE( fs::exists(sync.remote / a ));
    REQUIRE_FALSE( fs::exists(sync.remote / b ));

    REQUIRE( s.index.get("dir") == sync_mode::SHALLOW );
    REQUIRE( s.state.get("dir") );
    REQUIRE( s.state.get(a) );
    REQUIRE_FALSE( s.state.get(b) );

}

TEST_CASE("file to dir", "[synchronizer]") {

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    auto sync = make_sync(local);

    const fs::path l = local ? sync.local : sync.remote;
    const fs::path r = local ? sync.remote : sync.local;

    write_text(l / test_name, test_content);
    write_text(r / test_name / "x.txt", test_content);

    plus50ms_last_write_time(l / test_name, r / test_name);

    synchronizer s(sync);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {test_name, lf::index(sync_mode::SHALLOW) } });
    s.run();

    REQUIRE( fs::status(r / test_name).type() == fs::file_type::regular );
    REQUIRE( read_text(r / test_name) == test_content );

}

TEST_CASE("dir to file", "[synchronizer]") {

    bool local = GENERATE(true, false);
    INFO( test_direction(local) );
    auto sync = make_sync(local);

    const fs::path l = local ? sync.local : sync.remote;
    const fs::path r = local ? sync.remote : sync.local;

    write_text(l / test_name / "x.txt", test_content);
    write_text(r / test_name, test_content);

    plus50ms_last_write_time(l / test_name, r / test_name);

    synchronizer s(sync);
    s.index = lf::index(sync_mode::UNSPECIFIED, { {test_name, lf::index(sync_mode::SHALLOW) } });
    s.run();

    REQUIRE( fs::status(r / test_name).type() == fs::file_type::directory );
    REQUIRE( read_text(r / test_name / "x.txt") == test_content );

}
