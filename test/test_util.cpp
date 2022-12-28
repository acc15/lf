#include "test_util.hpp"

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

}
