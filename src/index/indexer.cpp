#include "index/indexer.hpp"

#include "fs/util.hpp"
#include "log/log.hpp"
#include "config/config.hpp"

namespace fs = std::filesystem;

namespace lf {

    indexer::indexer(): _config(config::load()) {
    }

    void indexer::process(const std::vector<std::string_view>& paths, std::optional<sync_mode> mode) {
        for (const std::string_view& p: paths) {
            process(p, mode);
        }
    }

    void indexer::process(std::string_view path_str, std::optional<sync_mode> mode) {

        fs::path path;
        try {
            path = normalize_path(path_str);
        } catch (const fs::filesystem_error& ex) {
            log.error() && log() << "unable to normalize path \"" << path_str << "\", error: " << ex.what() << log::end;
            _success = false;
            return;
        }

        config::sync_entry_vec matches = _config.find_most_specific_local_matches(path);
        for (const auto* e: matches) {

            const config::sync& sync = *e;

            const fs::path rel_path = relative_path(path, sync.local);
            tracked_index& index = load_index(sync.index);
            if (mode.has_value()) {
                log.info() && log() 
                    << "set " << path << " mode to " << mode.value() 
                    << " in \"" << sync.name << "\" sync index " << sync.index 
                    << log::end;
                index.set(rel_path, mode.value());
            } else {
                log.info() && log() 
                    << "rm " << path << " from \"" << sync.name << "\" sync index " << sync.index 
                    << log::end;
                index.remove(rel_path);
            }
        }

    }

    bool indexer::save_changes() {
        for (index_entry& e: _indexes) {
            save_entry(e.first, e.second.first, e.second.second);
        }
        return _success;
    }

    tracked_index& indexer::load_index(const std::filesystem::path& index_path) {
        index_map::iterator it = _indexes.find(index_path);
        if (it != _indexes.end()) {
            return it->second.first;
        }

        const auto r = _indexes.emplace(index_path, index_pair {});
        if (r.second) {
            init_entry(r.first->first, r.first->second.first, r.first->second.second);
        }
        return r.first->second.first;
    }

    void indexer::init_entry(const std::filesystem::path& path, tracked_index& index, adv_fstream& file) {

        try {
            create_parent_dirs(path);
            open_and_lock<tree_binary_format, lf::index>(path, file, OPEN_READ_WRITE_LOCK);
        } catch (const std::runtime_error& e) {
            _success = false;
            log.error() && log() << e.what() << log::end;
            return;
        }

        if (file.tellg() == 0) {
            // new empty file, skip loading
            return;
        }

        file.seekg(0);
        try {
            load_file<tree_binary_format>(path, file, index.root);
        } catch (const std::runtime_error& e) {
            log.debug() && log() << e.what() << log::end;
        }

    }

    void indexer::save_entry(const std::filesystem::path& path, tracked_index& index, adv_fstream& file) {
        if (!index.changed || !file) {
            return;
        }

        file.truncate();

        try {
            save_file<tree_binary_format, lf::index>(path, file, index.root);
        } catch (const std::runtime_error& e) {
            log.error() && log() << e.what() << log::end;
            _success = false;
        }
    }

}
