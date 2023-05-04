#include "index/indexer.hpp"

#include "fs/util.hpp"
#include "log/log.hpp"
#include "config/config.hpp"

namespace fs = std::filesystem;

namespace lf {

    indexer::indexer(): config(config::load()) {
        load_index();
    }

    void indexer::set_batch(const std::vector<std::string_view>& paths, sync_mode mode) {
        for (const std::string_view& p: paths) {
            const auto pi = resolve(p);
            if (!pi.has_value()) {
                continue;
            }
            set(pi->index, mode);
        }
    }

    void indexer::move(const std::filesystem::path& from, const std::filesystem::path& to) {
        log.info() && log() 
            << "moving " << from << " to " << to
            << " in index " << config.index << log::end;
        tree<sync_mode>* from_node = index.root.node(from);
        // TODO implement
    }

    void indexer::set(const std::filesystem::path& path, sync_mode mode) {
        log.info() && log() 
            << "set " << path << " mode to " << mode
            << " in index " << config.index << log::end;
        index.set(path, mode);
    }

    void indexer::remove(const std::filesystem::path& path) {
        log.info() && log() << "rm " << path << " from index " << config.index << log::end;
        index.remove(path);
    }

    std::optional<indexer::path_info> indexer::resolve(std::string_view path_str) {
        path_info pi;
        
        try {
            pi.abs = normalize_path(path_str);
        } catch (const fs::filesystem_error& ex) {
            log.error() && log() << "unable to normalize path \"" << path_str << "\", error: " << ex.what() << log::end;
            success = false;
            return std::nullopt;
        }

        if (!is_subpath(pi.abs, config.local)) {
            log.error() && log()
                << "path " << pi.abs << " is outside of local sync directory " 
                << config.local << log::end;
            success = false;
            return std::nullopt;
        }

        pi.index = relative_path(pi.abs, config.local);
        return pi;
    }

    bool indexer::save_changes() {
        save_index();
        return success;
    }

    void indexer::load_index() {
        try {
            create_parent_dirs(config.index);
            open_and_lock<tree_binary_format, lf::index>(config.index, file, OPEN_READ_WRITE_LOCK);
        } catch (const std::runtime_error& e) {
            success = false;
            log.error() && log() << e.what() << log::end;
            return;
        }

        if (file.tellg() == 0) {
            // new empty file, skip loading
            return;
        }

        file.seekg(0);
        try {
            load_file<tree_binary_format>(config.index, file, index.root);
        } catch (const std::runtime_error& e) {
            log.debug() && log() << e.what() << log::end;
        }
    }

    void indexer::save_index() {
        if (!index.changed || !file) {
            return;
        }

        file.truncate();

        try {
            save_file<tree_binary_format, lf::index>(config.index, file, index.root);
        } catch (const std::runtime_error& e) {
            log.error() && log() << e.what() << log::end;
            success = false;
        }
    }

}
