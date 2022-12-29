#include "index/indexer.hpp"

#include "fs/path.hpp"
#include "io/log.hpp"
#include "config/config.hpp"
#include "io/serialization.hpp"
#include "tree/tree_binary.hpp"

namespace fs = std::filesystem;

namespace lf {

    bool indexer::process(const std::span<const char*> paths, sync_mode mode) {
        const config cfg = config::load();

        bool success = true;
        for (std::string_view path: paths) {
            success &= process_path(cfg, path, mode);
        }
        return success && save_changes();
    }

    bool indexer::process_path(const config& cfg, std::string_view path_str, sync_mode mode) {

        fs::path path;
        try {
            path = normalize_path(path_str);
        } catch (const fs::filesystem_error& ex) {
            log.error() && log() << "unable to normalize path \"" << path_str << "\", error: " << ex.what() << std::endl;
            return false;
        }

        fs::file_status status = fs::status(path);
        if (mode == sync_mode::RECURSIVE && status.type() != fs::file_type::directory) {
            log.error() && log() << "recursive can be used only for existing directories, but " 
                << path << " doesn't denote a directory" 
                << std::endl;
            return false;
        }
        
        if (mode != sync_mode::NONE && status.type() == fs::file_type::not_found) {
            log.error() && log() << "path " << path << " doesn't exists" << std::endl;
            return false;
        }

        config::match_vec matches = cfg.find_most_specific_matches(path);
        for (const auto* e: matches) {
            log.info() && log() << "set " << path << " mode to " << mode << " in \"" << e->first << "\" sync index " << e->second.index << std::endl;
            set_index_mode(e->second, relative_path(path, e->second.local), mode);
        }
        return true;
    }

    bool indexer::save_changes() const {
        bool success = true;
        for (const auto& p: _indexes) {
            if (p.second.second) {
                try {
                    save_file<index_tree>(p.first, p.second.first);
                } catch (const std::runtime_error& e) {
                    log.error() && log() << e.what() << std::endl;
                    success = false;
                }
            }
        }
        return success;
    }

    void indexer::set_index_mode(const config_sync& sync, const std::filesystem::path& rel_path, sync_mode mode) {
        index_change& change = load_index(sync.index);
        change.second |= change.first.set(rel_path, mode);
    }

    indexer::index_change& indexer::load_index(const std::filesystem::path& index_path) {
        index_map::iterator it = _indexes.find(index_path);
        if (it != _indexes.end()) {
            return it->second;
        }

        const auto emplace_result = _indexes.emplace(index_path, std::make_pair(index_tree {}, false));
        if (emplace_result.second) {
            try {
                load_file<index_tree>(index_path, emplace_result.first->second.first);
            } catch (const std::runtime_error& e) {
                log.debug() && log() << e.what() << std::endl;
            }
        }
        return emplace_result.first->second;
    }

}
