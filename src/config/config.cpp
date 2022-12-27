#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <yaml-cpp/yaml.h>

#include "config.hpp"
#include "../io/log.hpp"

namespace fs = std::filesystem;

namespace lf {

    void load_config_sync(const std::string& name, const YAML::Node node, config& config) {

        fs::path local = node["local"].as<std::string>();
        fs::path state = node["state"].as<std::string>();
        fs::path remote = node["remote"].as<std::string>();
        fs::path index = node["index"].as<std::string>();

        if (local.is_relative()) {
            log.warn() && log() << name << ".local must be absolute path" << std::endl;
            return;
        }

        if (remote.is_relative()) {
            log.warn() && log() << name << ".remote must be absolute path" << std::endl;
            return;
        }

        if (index.is_relative()) {
            index = remote / index;
        }
        
        if (state.is_relative()) {
            state = local / state;
        }

        const auto result = config.emplace(name, config_sync { 
            .local = std::move(local),
            .remote = std::move(remote),
            .state = std::move(state),
            .index = std::move(index)
        });

        if (!result.second) {
            log.warn() && log() << "duplicate sync \"" << name << "\" entry" << std::endl;
        }
    }

    std::istream& operator>>(std::istream& s, with_format<format::YAML, config&> dest) {
        dest.value.clear();

        try {
            YAML::Node node = YAML::Load(s);
            for (const auto& pair: node) {
                const std::string name = pair.first.as<std::string>();
                try {
                    load_config_sync(name, pair.second, dest.value);
                } catch (const std::runtime_error& e) {
                    log.warn() && log() << "unable to convert sync \"" << name << "\" entry" << std::endl;
                }
            }
        } catch (const std::runtime_error& e) {
            log.error() && log() << "unable to read YAML: " << e.what() << std::endl;
        }
        
        if (dest.value.empty()) {
            log.error() && log() << "config doesn't have any valid sync entry" << std::endl;
            s.setstate(std::istream::failbit);
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

}