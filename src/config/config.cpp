#include <cstdlib>
#include <string>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "config.hpp"

namespace fs = std::filesystem;

namespace lf {

    std::istream& operator>>(std::istream& s, with_format_and_errors<format::YAML, config&> dest) {
        dest.value.clear();

        try {
            YAML::Node node = YAML::Load(s);
            
            for (const auto& pair: node) {
                std::string sync_name = pair.first.as<std::string>();
                
                const auto& sync_node = pair.second;
                fs::path index = sync_node["index"].as<std::string>();
                fs::path local = sync_node["local"].as<std::string>();
                fs::path remote = sync_node["remote"].as<std::string>();
                if (local.is_relative()) {
                    dest.err << sync_name << ".local must be absolute path" << errors::end;
                    continue;
                }
                if (remote.is_relative()) {
                    dest.err << sync_name << ".remote must be absolute path" << errors::end;
                    continue;
                }
                if (index.is_relative()) {
                    index = local / index;
                }

                const auto result = dest.value.emplace(sync_name, config_sync { 
                    .index = std::move(index),
                    .local = std::move(local),
                    .remote = std::move(remote)
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

        const fs::path rel_path = fs::path("lf") / "lf.yaml";

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

    config load_config(errors& err) {
        config config;
        
        const fs::path path = get_config_path();
        err.loc.source = path.string();

        std::ifstream file(path);  
        if (!file) {
            err << "config file doesn't exists" << errors::end;
            return config;
        }

        file >> with_ref_format<format::YAML>(config, err);
        if (config.empty()) {
            err << "config doesn't have any declared sync" << errors::end;
            return config;
        }

        return config;
    }

    config load_config() {
        errors err(data_location{});
        return load_config(err);
    }

}