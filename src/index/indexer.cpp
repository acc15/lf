#include "index/indexer.hpp"

#include "fs/util.hpp"
#include "log/log.hpp"
#include "config/config.hpp"

namespace fs = std::filesystem;

namespace lf {

    indexer::indexer(): _config(config::load()) {
        init_index();
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

        if (!is_subpath(path, _config.local)) {
            log.error() && log()
                << "path " << path << " is outside of local sync directory " 
                << _config.local << log::end;
            _success = false;
            return;
        }

        const fs::path rel_path = relative_path(path, _config.local);
        if (mode.has_value()) {
            log.info() && log() 
                << "set " << path << " mode to " << mode.value() 
                << " in index " << _config.index 
                << log::end;
            _index.set(rel_path, mode.value());
        } else {
            log.info() && log() 
                << "rm " << path << " from index " << _config.index 
                << log::end;
            _index.remove(rel_path);
        }
    }

    bool indexer::save_changes() {
        save_index();
        return _success;
    }

    void indexer::init_index() {
        try {
            create_parent_dirs(_config.index);
            open_and_lock<tree_binary_format, lf::index>(_config.index, _file, OPEN_READ_WRITE_LOCK);
        } catch (const std::runtime_error& e) {
            _success = false;
            log.error() && log() << e.what() << log::end;
            return;
        }

        if (_file.tellg() == 0) {
            // new empty file, skip loading
            return;
        }

        _file.seekg(0);
        try {
            load_file<tree_binary_format>(_config.index, _file, _index.root);
        } catch (const std::runtime_error& e) {
            log.debug() && log() << e.what() << log::end;
        }
    }

    void indexer::save_index() {
        if (!_index.changed || !_file) {
            return;
        }

        _file.truncate();

        try {
            save_file<tree_binary_format, lf::index>(_config.index, _file, _index.root);
        } catch (const std::runtime_error& e) {
            log.error() && log() << e.what() << log::end;
            _success = false;
        }
    }

}
