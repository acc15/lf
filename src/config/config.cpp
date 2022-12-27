#include <cstdlib>
#include <string>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "config.hpp"
#include "../io/log.hpp"

namespace fs = std::filesystem;

namespace lf {

    std::istream& operator>>(std::istream& s, with_format_and_errors<format::YAML, config&> dest) {
        dest.value.clear();

        try {
            YAML::Node node = YAML::Load(s);
            
            for (const auto& pair: node) {
                std::string sync_name = pair.first.as<std::string>();
                
                const auto& sync_node = pair.second;
                fs::path local = sync_node["local"].as<std::string>();
                fs::path state = sync_node["state"].as<std::string>();
                fs::path remote = sync_node["remote"].as<std::string>();
                fs::path index = sync_node["index"].as<std::string>();

                if (local.is_relative()) {
                    dest.err << sync_name << ".local must be absolute path" << errors::end;
                    continue;
                }

                if (remote.is_relative()) {
                    dest.err << sync_name << ".remote must be absolute path" << errors::end;
                    continue;
                }

                if (index.is_relative()) {
                    index = remote / index;
                }
                
                if (state.is_relative()) {
                    state = local / state;
                }

                const auto result = dest.value.emplace(sync_name, config_sync { 
                    .local = std::move(local),
                    .remote = std::move(remote),
                    .state = std::move(state),
                    .index = std::move(index)
                });

                if (!result.second) {
                    dest.err << "duplicate sync \"" << sync_name << "\" entry" << errors::end;
                }

            }

        } catch (const std::runtime_error& e) {
            dest.err << "unable to read YAML: " << e.what() << errors::end;
        }
        return s;
    }
    
    fs::path get_config_path() {
        const char* config_path = std::getenv("LF_CONFIG");
        if (config_path != nullptr) {
            return config_path;
        }

        const fs::path rel_path = "lf/lf.yaml";

#if __linux__
        const char* home = std::getenv("HOME");
        if (home != nullptr) {
            return fs::path(home) / ".config" / rel_path;
        }
        return fs::path("/etc") / rel_path;
#elif _WIN32
        const char* local_app_data = std::getenv("LOCALAPPDATA");
        if (local_app_data != nullptr) {
            return fs::path(local_app_data) / rel_path;
        }
        const char* program_data = std::getenv("PROGRAMDATA")
        if (program_data != nullptr) {
            return fs::path(program_data) / rel_path;
        }
        return fs::path("C:\\ProgramData") / rel_path;
#elif __APPLE__
        const char* home = std::getenv("HOME");
        return fs::path(home != nullptr ? home : "/") / "Library" / "Preferences" / rel_path;
#endif
    }

    config load_config() {
        config config;
        
        const fs::path path = get_config_path();
        log.info() && log() << "loading config from " << path << "..." << std::endl;

        std::ifstream file(path);  
        if (!file) {
            log.error() && log() << "config file doesn't exists" << std::endl;
            return config;
        }

        errors err(path.string());
        file >> with_ref_format<format::YAML>(config, err);
        if (config.empty()) {
            log.error() && log() << "config doesn't have any declared sync" << std::endl;
            return config;
        }

        return config;
    }

}