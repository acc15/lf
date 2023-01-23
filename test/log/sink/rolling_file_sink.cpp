#include <catch2/catch_test_macros.hpp>

#include <set>

#include "log/sink/rolling_file_sink.hpp"

namespace fs = std::filesystem;

struct name_compare {
    bool operator()(const std::string& a, const std::string& b) const {
        return a.size() < b.size() || (a.size() == b.size() && a < b);
    }
};

TEST_CASE("cmp", "[rolling_file_sink]") {

    std::set<std::string, name_compare> names = {
        "test.log.2024_01_30.9",
        "test.log.2023_01_02.9",
        "test.log.2023_01_02.10",
        "test.log.2023_01_02.20",
    };

    for (const auto& n: names) {
        WARN(n);
    }   

}

TEST_CASE("write", "[rolling_file_sink]") {
    
    std::string log_name = "test.log";
    fs::path log_path = fs::temp_directory_path() / "lf_test" / "rolling_file_sink" / log_name;

    fs::path parent = log_path.parent_path(); 
    fs::remove_all(parent);
    fs::create_directories(parent);

    lf::rolling_file_sink sink(log_path, 10, 2, false);

    sink.send(lf::log_message { .level = lf::INFO, .timestamp = {}, .text = "1abcdefghi" });
    sink.send(lf::log_message { .level = lf::INFO, .timestamp = {}, .text = "2abcdefghi" });
    sink.send(lf::log_message { .level = lf::INFO, .timestamp = {}, .text = "3abcdefghi" });
    sink.send(lf::log_message { .level = lf::INFO, .timestamp = {}, .text = "4abcdefghi" });
    sink.send(lf::log_message { .level = lf::INFO, .timestamp = {}, .text = "5abcdefghi" });
    
    std::set<std::string> filenames;
    for (const fs::directory_entry& e: fs::directory_iterator(parent)) {
        filenames.insert(e.path().filename().string());
    }

    REQUIRE( filenames == std::set<std::string> { log_name, log_name + ".1", log_name + ".2" } );


}