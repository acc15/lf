#include <catch2/catch_test_macros.hpp>

#include <set>

#include "log/sink/rolling_file_sink.hpp"

#include "test_util.hpp"

namespace fs = std::filesystem;

using namespace lf;

const std::string log_name = "test.log";
const fs::path log_dir = fs::temp_directory_path() / "lf_test" / "rolling_file_sink";
const fs::path log_file = log_dir / log_name;

TEST_CASE("rollover", "[rolling_file_sink]") {
    
    fs::remove_all(log_dir);

    rolling_file_sink sink(log_file, 10, 2, false);

    sink.send(log_message { .level = INFO, .timestamp = {}, .text = "1abcdefghi" });
    sink.send(log_message { .level = INFO, .timestamp = {}, .text = "2abcdefghi" });
    sink.send(log_message { .level = INFO, .timestamp = {}, .text = "3abcdefghi" });
    sink.send(log_message { .level = INFO, .timestamp = {}, .text = "4abcdefghi" });
    sink.send(log_message { .level = INFO, .timestamp = {}, .text = "5abcdefghi" });
    
    std::set<std::string> filenames;
    for (const fs::directory_entry& e: fs::directory_iterator(log_dir)) {
        filenames.insert(e.path().filename().string());
    }

    REQUIRE( filenames == std::set<std::string> { log_name, log_name + ".3", log_name + ".4" } );

    REQUIRE( read_text(log_file) == "5abcdefghi" );
    REQUIRE( read_text(log_dir / (log_name + ".3")) == "3abcdefghi" );
    REQUIRE( read_text(log_dir / (log_name + ".4")) == "4abcdefghi" );

}

TEST_CASE("append", "[rolling_file_sink]") {
    write_text(log_file, "abc");

    {
        rolling_file_sink sink(log_file, 10, 2, false);
        sink.send(log_message { .level = INFO, .timestamp = {}, .text = "xyz" });
    }
    
    REQUIRE( read_text(log_file) == "abcxyz" );
}