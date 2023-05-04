#include <string_view>
#include <fstream>
#include <unordered_map>

#include "log/log.hpp"

#include "config/config.hpp"
#include "config/config_parser.hpp"
#include "fs/os_path.hpp"
#include "fs/util.hpp"
#include "io/format_stream.hpp"

namespace fs = std::filesystem;

namespace lf {

    const char* const config::name = "config";
    const char* const config::env_name = "LF_CONFIG";

    const std::unordered_map<std::string_view, void (*)(config&, std::string_view)> binder_map = {
        { "local",  [](config& c, std::string_view v) { c.local = v; } },
        { "remote", [](config& c, std::string_view v) { c.remote = v; } },
        { "index",  [](config& c, std::string_view v) { c.index = v; } },
        { "state",  [](config& c, std::string_view v) { c.state = v; } }
    };

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

    std::istream& operator>>(std::istream& s, with_format<config_format, config> dest) {
        config& cfg = dest.value;
        parse_config(s, [&cfg](const config_entry& e) {
            if (e.value.empty()) {
                log.warn() && log() << "empty value for key \"" << e.key << "\" at line " << e.line << log::end;
                return;
            }
            const auto bind_fn = binder_map.find(e.key);
            if (bind_fn == binder_map.end()) {
                log.warn() && log() << "invalid key \"" << e.key << "\" at line " << e.line << log::end;
            } else {
                bind_fn->second(cfg, e.value);
            }
        });
        if (!cfg.index.is_absolute()) {
            cfg.index = cfg.remote / cfg.index;
        }
        if (!cfg.state.is_absolute()) {
            cfg.state = cfg.local / cfg.state;
        }

        bool valid = check_config_path_absolute(cfg.local, "local");
        valid &= check_config_path_absolute(cfg.remote, "remote");
        valid &= check_config_path_specified(cfg.index, "index");
        valid &= check_config_path_specified(cfg.state, "state");
        if (!valid) {
            s.setstate(std::ios::failbit);
        }
        return s;
    }
    
    fs::path config::get_path() {
        const char* config_path = std::getenv(env_name);
        return config_path != nullptr ? config_path : get_default_path();
    }

    fs::path config::get_default_path() {
        return get_os_base_path(os_path_kind::CONFIG) / "lf" / "lf.conf";
    }

    config config::load() {
        const fs::path path = get_path();

        std::ifstream file(path);
        if (!file) {
            throw_fs_error("unable to open config file", path);
        }

        config cfg;
        if (!(file >> read_as<config_format>(cfg))) {
            throw std::runtime_error(format_stream() << "config file at " << path << " isn't valid");
        }
        return cfg;
    }

}