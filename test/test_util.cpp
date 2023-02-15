#include <catch2/interfaces/catch_interfaces_capture.hpp>
#include "test_util.hpp"
#include "fs/util.hpp"

#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace lf {

    const fs::path test_dir_path = fs::path(__FILE__).parent_path();
    
#ifdef _WIN32
    const fs::path test_root_path = "C:\\";
#else
    const fs::path test_root_path = "/";
#endif

    fs::path create_temp_test_dir(const fs::path& suffix) {
        fs::path p = fs::temp_directory_path() / "lf_test" / Catch::getResultCapture().getCurrentTestName();
        if (!suffix.empty()) {
            p /= suffix;
        }

        fs::create_directories(p);
        for (auto child: fs::directory_iterator(p)) {
            fs::remove_all(child);
        }
        return p;
    }

    std::string test_root_str(std::string_view p) {
        return (test_root_path / p).string();
    }

    void write_text(const std::filesystem::path& path, const std::string& text) {
        fs::create_directories(path.parent_path());
        std::ofstream s(path);
        s << text;
    }

    std::string read_text(const std::filesystem::path& path) {
        std::ifstream s(path);
        std::stringstream buf;
        s >> buf.rdbuf();
        return buf.str();
    }

}
