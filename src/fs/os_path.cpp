#include "fs/os_path.hpp"

namespace fs = std::filesystem;

namespace lf {

    std::filesystem::path get_os_base_path(os_path_kind kind) {
#if __linux__
        const char* home = std::getenv("HOME");
        if (home != nullptr) {
            return fs::path(home) / (kind == os_path_kind::LOG ? ".cache" : ".config");
        }
        return kind == os_path_kind::LOG ? "/var/log" : "/etc";
#elif _WIN32
        const char* local_app_data = std::getenv("LOCALAPPDATA");
        if (local_app_data != nullptr) {
            return fs::path(local_app_data);
        }
        const char* program_data = std::getenv("PROGRAMDATA");
        if (program_data != nullptr) {
            return fs::path(program_data);
        }
        return fs::path("C:\\ProgramData");
#elif __APPLE__
        const char* home = std::getenv("HOME");
        return fs::path(home != nullptr ? home : "/") / "Library" / (kind == os_path_kind::LOG ? "Logs" : "Preferences");
#else
#       error Unknown platform!
#endif
    }

}