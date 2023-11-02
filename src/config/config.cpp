#include <string_view>
#include <fstream>
#include <unordered_map>

#include "log/log.hpp"

#include "config/config.hpp"
#include "fs/os_path.hpp"
#include "fs/util.hpp"
#include "io/format_stream.hpp"
#include "util/string.hpp"

namespace fs = std::filesystem;

namespace lf {

    const char* const config::name = "config";
    const char* const config::env_name = "LF_CONFIG";

    bool check_config_path_specified(const std::filesystem::path& p, const char* key) {
        if (p.empty()) {
            log.error() && log() << key << " path isn't specified" << log::end;
            return false;
        }
        return true;
    }

    bool check_config_path_absolute(const std::filesystem::path& p, const char* key) {
        if (!check_config_path_specified(p, key)) {
            return false;
        }
        if (!p.is_absolute()) {
            log.error() && log() << key << " path must be absolute" << log::end;
            return false;
        }
        return true;
    }
    
    fs::path config::get_path() {
        const char* config_path = std::getenv(env_name);
        return config_path != nullptr ? config_path : get_default_path();
    }

    fs::path config::get_default_path() {
        return get_os_base_path(os_path_kind::CONFIG) / "lf" / "lf.conf";
    }

    void validate_and_set_default(config& cfg) {
        if (!cfg.left.is_absolute()) {
            throw std::logic_error(format_stream() << "config left path must be absolute path: " << cfg.left);
        }
        if (!cfg.right.is_absolute()) {
            throw std::logic_error(format_stream() << "config right path must be absolute path: " << cfg.right);
        }
        if (cfg.index.empty()) {
            throw std::logic_error(format_stream() << "config index path can't be empty");
        }
        if (cfg.state.empty()) {
            throw std::logic_error(format_stream() << "config state path can't be empty");
        }
        if (!cfg.index.is_absolute()) {
            cfg.index = cfg.right / cfg.index;
        }
        if (!cfg.state.is_absolute()) {
            cfg.state = cfg.left / cfg.state;
        }
    }

    config config::parse(std::istream& stream) {
        config cfg;

        size_t line_no = 0;
        std::string line;
        while (std::getline(stream, line)) {
            ++line_no;

            const auto sv = trim(line);
            if (sv.empty() || sv.starts_with("#")) {
                continue;
            }
            const auto eq_pos = sv.find('=');
            if (eq_pos == std::string_view::npos) {
                throw std::logic_error(format_stream() << "invalid input line at " << line_no << ": " << line);
            }
            
            const auto key = rtrim(sv.substr(0, eq_pos));
            if (key.empty()) {
                throw std::logic_error(format_stream() << "empty key at " << line_no << ": " << line);
            }

            const auto value = ltrim(sv.substr(eq_pos + 1));
            if (key == "left") {
                cfg.left = value;
            } else if (key == "right") {
                cfg.right = value;
            } else if (key == "index") {
                cfg.index = value;
            } else if (key == "state") {
                cfg.state = value;
            } else {
                throw std::logic_error(format_stream() << "unknown config key at " << line_no << ": " << key);
            }

        }
        validate_and_set_default(cfg);
        return cfg;
    }

    config config::load() {
        const fs::path path = get_path();
        std::ifstream file(path);
        if (!file) {
            throw_fs_error("unable to open config file", path);
        }
        return parse(file);
    }

}