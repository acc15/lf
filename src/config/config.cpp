#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <algorithm>

#include "fs/serialization.hpp"
#include "log/log.hpp"

#include "config/config.hpp"
#include "config/config_parser.hpp"
#include "fs/os_path.hpp"

namespace fs = std::filesystem;

namespace lf {

    bool check_config_path_specified(const std::string& name, const std::filesystem::path& p, const char* key) {
        if (p.empty()) {
            log.error() && log() << "sync \"" << name << "\" " << key << " path isn't specified" << log::end;
            return false;
        }
        return true;
    }

    bool check_config_path_absolute(const std::string& name, const std::filesystem::path& p, const char* key) {
        if (!check_config_path_specified(name, p, key)) {
            return false;
        }
        if (!p.is_absolute()) {
            log.error() && log() << "sync \"" << name << "\" " << key << " path must be absolute" << log::end;
            return false;
        }
        return true;
    }

    std::istream& operator>>(std::istream& s, with_format<config_format, config> dest) {
        config& cfg = dest.value;
        cfg.syncs.clear();
        
        parse_config(s, [&cfg](const config_entry& e) {
            if (e.section.empty()) {
                log.warn() && log() << "empty section at line " << e.line << log::end;
                return;
            }

            if (e.value.empty()) {
                log.warn() && log() << "empty value for key \"" << e.key << "\" at line " << e.line << log::end;
                return;
            }

            config::sync_vec::iterator it = cfg.find_by_name(e.section);
            config::sync& sync = it == cfg.syncs.end() 
                ? cfg.syncs.emplace_back(config::sync { .name = e.section })
                : *it;

            if (e.key == "local") {
                sync.local = e.value;
            } else if (e.key == "remote") {
                sync.remote = e.value;
            } else if (e.key == "index") {
                sync.index = e.value;
            } else if (e.key == "state") {
                sync.state = e.value;
            } else {
                log.warn() && log() << "invalid key \"" << e.key << "\" at line " << e.line << log::end;
            }
        });

        auto it = cfg.syncs.begin();
        while (it != cfg.syncs.end()) {
            config::sync& sync = *it;
            
            bool valid = check_config_path_absolute(sync.name, sync.local, "local");
            valid &= check_config_path_absolute(sync.name, sync.remote, "remote");
            valid &= check_config_path_specified(sync.name, sync.index, "index");
            valid &= check_config_path_specified(sync.name, sync.state, "state");

            if (valid) {
                if (sync.index.is_relative()) {
                    sync.index = sync.remote / sync.index;
                }
                if (sync.state.is_relative()) {
                    sync.state = sync.local / sync.state;
                }
                ++it;
            } else {
                it = cfg.syncs.erase(it);
            }
            
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
        config cfg = load_file<config>(path);
        if (cfg.syncs.empty()) {
            throw std::runtime_error(format_stream() << "config file at " << path << " doesn't have any valid sync entry");
        }
        return cfg;
    }

    config::sync_entry_vec config::find_name_matches(const std::vector<std::string_view>& names) const {
        sync_entry_vec result;
        if (names.empty()) {
            std::transform(syncs.begin(), syncs.end(), std::back_inserter(result), [](const auto& ref) { return &ref; });
            return result;
        }
        for (std::string_view n: names) {
            const auto it = find_by_name(n);
            if (it != syncs.end()) {
                result.push_back(&(*it));
            }
        }
        return result;
    }

    config::sync_entry_match_map config::find_local_matches(const std::filesystem::path &p) const {
        sync_entry_match_map result;
        for (const auto& e: syncs) {
            const std::filesystem::path& local_path = e.local;
            if (!is_subpath(p, local_path)) {
                continue;
            }
            result[std::distance(local_path.begin(), local_path.end())].push_back(&e);
        }
        return result;
    }

    config::sync_entry_vec config::find_most_specific_local_matches(const std::filesystem::path& p) const {
        sync_entry_match_map m = find_local_matches(p);
        if (m.empty()) {
            log.error() && log() << "no configured syncs found for path " << p << log::end;
            return config::sync_entry_vec();
        }
        return m.rbegin()->second;
    }

    config::sync_vec::const_iterator config::find_by_name(std::string_view name) const {
        return std::find_if(syncs.begin(), syncs.end(), [name](const auto& e) { return e.name == name; });
    }

    config::sync_vec::iterator config::find_by_name(std::string_view name) {
        return std::find_if(syncs.begin(), syncs.end(), [name](const auto& e) { return e.name == name; });
    }

    const char* const config::name = "config";
    const char* const config::env_name = "LF_CONFIG";

}