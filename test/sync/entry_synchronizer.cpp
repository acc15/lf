#include <catch2/catch_test_macros.hpp>

#include "sync/entry_synchronizer.hpp"
#include "test_util.hpp"

using namespace lf;

namespace fs = std::filesystem;

TEST_CASE("entry_synchronizer: create_dir_if_not_exists", "[entry_synchronizer]") {
    
    config::sync sync;
    lf::index index;
    lf::tracked_state state;

    synchronizer s(sync, index, state);
    sync_item item = {};

    entry_synchronizer es(s, item);

    fs::path root_path = create_temp_test_dir(fs::path("entry_synchronizer") / "create_dir_if_not_exists");
    fs::path file_path = fs::path("a") / "b";
    fs::path dir_path = file_path / "c" / "d"; 

    write_text(root_path / file_path, "abc");

    path_info pi(true, root_path, dir_path);
    REQUIRE( es.create_dir_if_not_exists(pi) );
    REQUIRE( fs::status(root_path / file_path).type() == fs::file_type::directory );
    REQUIRE( fs::status(root_path / dir_path).type() == fs::file_type::directory );

    std::ostringstream ss;
    ss << "deleted local file " << file_path << ", created local directory " << dir_path;
    REQUIRE( es.message() == ss.str() );

}