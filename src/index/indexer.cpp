#include "index/indexer.hpp"

#include "fs/util.hpp"
#include "log/log.hpp"
#include "config/config.hpp"

namespace fs = std::filesystem;

namespace lf {

    void indexer::process(const config& cfg, const std::vector<std::string_view>& paths, std::optional<sync_mode> mode) {
        for (const std::string_view& p: paths) {
            process(cfg, p, mode);
        }
    }

    void indexer::process(const config& cfg, std::string_view path_str, std::optional<sync_mode> mode) {

        fs::path path;
        try {
            path = normalize_path(path_str);
        } catch (const fs::filesystem_error& ex) {
            log.error() && log() << "unable to normalize path \"" << path_str << "\", error: " << ex.what() << log::end;
            success = false;
            return;
        }

        config::sync_entry_vec matches = cfg.find_most_specific_local_matches(path);
        for (const auto* e: matches) {
            
            const std::string& name = e->first;
            const config::sync& sync = e->second;

            const fs::path rel_path = relative_path(path, sync.local);
            tracked_index& index = load_index(sync.index);
            if (mode.has_value()) {
                log.info() && log() << "set " << path << " mode to " << mode.value() << " in \"" << name << "\" sync index " << sync.index << log::end;
                index.set(rel_path, mode.value());
            } else {
                log.info() && log() << "rm " << path << " from \"" << name << "\" sync index " << sync.index << log::end;
                index.remove(rel_path);
            }
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

    tracked_index& indexer::load_index(const std::filesystem::path& index_path) {
        index_map::iterator it = _indexes.find(index_path);
        if (it != _indexes.end()) {
            return it->second;
        }

        const auto r = _indexes.emplace(index_path, tracked_index {});
        if (r.second) {
            try {
                r.first->second.load(index_path);
            } catch (const std::runtime_error& e) {
                log.debug() && log() << e.what() << log::end;
            }
        }
        return r.first->second;
    }

}
