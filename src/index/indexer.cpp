#include "index/indexer.hpp"

#include "fs/util.hpp"
#include "log/log.hpp"
#include "config/config.hpp"

namespace fs = std::filesystem;

namespace lf {

    void indexer::process(const config& cfg, const std::vector<std::string_view>& paths, sync_mode mode) {
        for (const std::string_view& p: paths) {
            process(cfg, p, mode);
        }
    }

    void indexer::process(const config& cfg, std::string_view path_str, sync_mode mode) {

        fs::path path;
        try {
            path = normalize_path(path_str);
        } catch (const fs::filesystem_error& ex) {
            log.error() && log() << "unable to normalize path \"" << path_str << "\", error: " << ex.what() << log::end;
            success = false;
            return;
        }

        fs::file_status status = fs::status(path);
        if (mode == sync_mode::RECURSIVE && status.type() != fs::file_type::directory) {
            log.error() && log() << "recursive can be used only for existing directories, but " 
                << path << " doesn't denote a directory" 
                << log::end;
            success = false;
            return;
        }
        
        if (mode != sync_mode::UNSPECIFIED && status.type() == fs::file_type::not_found) {
            log.error() && log() << "path " << path << " doesn't exists" << log::end;
            success = false;
            return;
        }

        config::sync_entry_vec matches = cfg.find_most_specific_local_matches(path);
        for (const auto* e: matches) {
            log.info() && log() << "set " << path << " mode to " << mode << " in \"" << e->first << "\" sync index " << e->second.index << log::end;
            set_index_mode(e->second, relative_path(path, e->second.local), mode);
        }

    }

    void indexer::save_changes() {
        for (auto& p: _indexes) {
            try {
                p.second.save_if_changed(p.first);
            } catch (const std::runtime_error& e) {
                log.error() && log() << e.what() << log::end;
                success = false;
            }
        }
    }

    bool indexer::is_successful() const {
        return success;
    }

    void indexer::set_index_mode(const config::sync& sync, const std::filesystem::path& rel_path, sync_mode mode) {
        load_index(sync.index).set(rel_path, mode);
    }

    tracked_index& indexer::load_index(const std::filesystem::path& index_path) {
        index_map::iterator it = _indexes.find(index_path);
        if (it != _indexes.end()) {
            return it->second;
        }

        const auto emplace_result = _indexes.emplace(index_path, tracked_index {});
        if (emplace_result.second) {
            try {
                emplace_result.first->second.load(index_path);
            } catch (const std::runtime_error& e) {
                log.debug() && log() << e.what() << log::end;
            }
        }
        return emplace_result.first->second;
    }

}
