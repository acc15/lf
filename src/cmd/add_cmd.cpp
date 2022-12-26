#include <filesystem>
#include <iostream>
#include <fstream>

#include "add_cmd.hpp"
#include "../index/index_entry.hpp"
#include "../fs/path.hpp"

namespace fs = std::filesystem;

namespace lf {

    const char* add_cmd::names[] = { "a", "add" };
    const cmd_desc add_cmd::desc = {names, "<file|shallow-dir/*|recursive-dir/**>+", "add specified file(s) to corresponding index file(s)"};

    add_cmd::add_cmd() : cmd(desc) {
    }

    int add_cmd::run(const std::span<const char*>& args) const {
        if (args.empty()) {
            std::cerr << desc;
            return 1;
        }

        config cfg = load_config();
        if (cfg.empty()) {
            return 1;
        }
        
        for (std::string_view path_str: args) {
            try {
                process_path(cfg, path_str);
            } catch (const fs::filesystem_error& ex) {
                std::cerr << "unable to add " << path_str << ", error: " << ex.what() << std::endl;
            }
        }
        return 0;
    }

    void add_cmd::process_path(const config& cfg, std::string_view path_str) const {
        index_sync_mode mode = index_sync_mode::SHALLOW;
        if (path_str.ends_with("**")) {
            mode = index_sync_mode::RECURSIVE;
            path_str.remove_suffix(2);
        } else if (path_str.ends_with("*")) {
            path_str.remove_suffix(1);
        }

        fs::path path = normalize_path(path_str);

        fs::file_status status = fs::status(path);
        if (status.type() != fs::file_type::directory) {
            if (mode == index_sync_mode::RECURSIVE) {
                std::cerr << "recursive mode must be set only for existing directory, but " << path << " doesn't denote a directory" << std::endl;
                return;
            }
            if (status.type() == fs::file_type::not_found) {
                std::cerr << "path " << path << " doesn't exists" << std::endl;
                return;
            }
        }

        std::cout << "adding " << path << " with mode " << mode << std::endl;
        for (const auto& sync_pair: cfg) {
            const config_sync& sync = sync_pair.second;
            if (!is_subpath(path, sync.local)) {
                continue;
            }
            add_path(sync, relative_path(path, sync.local), mode);
        }

    }

    void add_cmd::add_path(const config_sync& sync, const std::filesystem::path& rel_path, index_sync_mode mode) const {
        // index_entry index;
        // std::fstream file(sync.index);
        // if (file.eof()) {
        //     errors err { data_location { .source = sync.index } };
        //     file >> with_ref_format(index, err);
        // }
        // index.set(rel_path, index_flags { .mode = mode });
    }

}