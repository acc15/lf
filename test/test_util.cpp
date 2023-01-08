#include "test_util.hpp"
#include "fs/path.hpp"

#include <fstream>

namespace lf {

    const std::filesystem::path test_dir_path = std::filesystem::path(__FILE__).parent_path();
    
#ifdef _WIN32
    const std::filesystem::path test_root_path = "C:\\";
#else
    const std::filesystem::path test_root_path = "/";
#endif

    std::string test_root_str(std::string_view p) {
        return (test_root_path / p).string();
    }

    void write_text(const std::filesystem::path& path, const std::string& text) {
        create_parent_dirs(path);
        std::ofstream s(path);
        s << text;
    }

    std::string read_text(const std::filesystem::path& path) {
        std::ifstream s(path);
        std::string str;
        s >> str;
        return str;
    }

}
