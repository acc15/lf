#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#include "add_cmd.hpp"
#include "../index/index_tree.hpp"
#include "../fs/path.hpp"
#include "../io/log.hpp"
#include "../io/serialization.hpp"

namespace fs = std::filesystem;

namespace lf {

    const char* add_cmd::names[] = { "a", "add" };
    const cmd_desc add_cmd::desc = {
        names, 
        "< [--recursive|-R] <recursive dirs>+ | [--shallow|-S] <file|dir>+ >", 
        "add specified file(s) to corresponding index file(s)"
    };

    add_cmd::add_cmd() : cmd(desc) {
    }

    int add_cmd::run(const std::span<const char*>& args) const {
        sync_mode mode = sync_mode::SHALLOW;
        
        std::span<const char*> non_opts = args;
        if (!non_opts.empty()) {
            std::string_view opt = args[0];
            if (opt == "--recursive" || opt == "-R") {
                mode = sync_mode::RECURSIVE;
                non_opts = non_opts.subspan(1);
            } else if (opt == "--shallow" || opt == "-S") {
                mode = sync_mode::SHALLOW;
                non_opts = non_opts.subspan(1);
            }
        }

        if (non_opts.empty()) {
            log.error() && log() << desc;
            return 1;
        }

        config cfg;
        if (!load_file<format::YAML>(get_config_path(), "config", cfg)) {
            return 1;
        }
        
        for (std::string_view path_str: args) {
            try {
                process_path(cfg, path_str, mode);
            } catch (const fs::filesystem_error& ex) {
                log.error() && log() << "unable to add " << path_str << ", error: " << ex.what() << std::endl;
            }
        }
        return 0;
    }

    void add_cmd::process_path(const config& cfg, std::string_view path_str, sync_mode mode) const {
        fs::path path = normalize_path(path_str);

        fs::file_status status = fs::status(path);
        if (mode == sync_mode::RECURSIVE && status.type() != fs::file_type::directory) {
            log.error() && log() << "recursive can be used only for existing directories, but " << path << " doesn't denote a directory" << std::endl;
            return;
        }
        
        if (mode != sync_mode::NONE && status.type() == fs::file_type::not_found) {
            log.error() && log() << "path " << path << " doesn't exists" << std::endl;
            return;
        }

        log.debug() && log() << "adding " << path << " with mode " << mode << std::endl;
        for (const auto& sync_pair: cfg) {
            const config_sync& sync = sync_pair.second;
            if (!is_subpath(path, sync.local)) {
                log.debug() && log() << "sync " << sync_pair.first << " skipped due to different base path: " << sync.local << std::endl;
                continue;
            }
            add_path(sync, relative_path(path, sync.local), mode);
        }

    }

    void add_cmd::add_path(const config_sync& sync, const std::filesystem::path& rel_path, sync_mode mode) const {
        // index_entry index;
        // std::fstream file(sync.index);
        // if (file.eof()) {
        //     errors err { data_location { .source = sync.index } };
        //     file >> with_ref_format(index, err);
        // }
        // index.set(rel_path, index_flags { .mode = mode });
    }

}