#include "sync/entry_synchronizer.hpp"

#include "io/time.hpp"
#include "io/file_type.hpp"
#include "fs/time.hpp"
#include "fs/util.hpp"

namespace fs = std::filesystem;

namespace lf {

    entry_synchronizer::entry_synchronizer(synchronizer& sync):
        sync(sync),
        item(std::move(sync.queue.back())),
        local(true, join_path(sync.config.local, item.path)),
        remote(false, join_path(sync.config.remote, item.path)),
        max_level(TRACE)
    {
        sync.queue.pop_back();
    }

    const std::filesystem::path& entry_synchronizer::path() const {
        return item.path;
    }

    std::string entry_synchronizer::message() const {
        return out.str();
    }

    log_level entry_synchronizer::level() const {
        return max_level;
    }

    void entry_synchronizer::run() {
        if (item.cleanup) {
            cleanup();
        } else if (item.mode == sync_mode::IGNORE) {
            log(DEBUG) << "ignored, skipping...";
            sync.state.remove(item.path);
        } else if (local.type == fs::file_type::not_found && remote.type == fs::file_type::not_found) {
            log(DEBUG) << "no file or directory exists on local and remote sides";
            sync.index.remove(item.path);
            sync.state.remove(item.path);
        } else if (remote.type == fs::file_type::not_found) {
            sync_not_found(local, remote);
        } else if (local.type == fs::file_type::not_found) {
            sync_not_found(remote, local);
        } else if (local.type == fs::file_type::directory && remote.type == fs::file_type::directory) {
            sync_dirs();
        } else {
            sync_with_timestamps();
        }
    }

    void entry_synchronizer::cleanup() {
        if (delete_if_empty(local)) {
            sync.state.remove(item.path);
        }
        delete_if_empty(remote);
        log(TRACE) << "post-cleanup finished";
    }

    void entry_synchronizer::sync_with_timestamps() {
        local.time = ntfs_last_write_time(local.path);
        remote.time = ntfs_last_write_time(remote.path);
        if (local.time == remote.time) {
            sync_same_time();
        } else if (local.time > remote.time) {
            sync_other(local, remote);
        } else {
            sync_other(remote, local);
        }
    }

    void entry_synchronizer::sync_same_time() {
        if (item.mode == sync_mode::UNSPECIFIED) {
            sync_skip();
        } else if (local.type == remote.type) {
            log(DEBUG) << "synced, both entries has same modification time (" << format_date_time(local.time) << ") and same type: " << local.type;
            sync.state.set(item.path, true, true);
        } else {
            log(WARN) << "CONFLICT, both entries has same modification time (" << format_date_time(local.time) 
                << "), but different types, local is " << local.type << ", remote is " << remote.type;
            sync.state.remove(item.path);
        }
    }

    void entry_synchronizer::sync_not_found(const path_info& src, const path_info& dst) {
        if (sync.state.get(item.path)) {
            sync_del(src, dst);
        } else {
            sync_new(src, dst);
        }
    }

    void entry_synchronizer::sync_del(const path_info& src, const path_info& dst) {
        log(INFO) << "was deleted in " << dst.name << ", deleting in " << src.name;
        fs::remove_all(src.path);
        sync.state.remove(item.path);
        sync.index.remove(item.path);
    }

    void entry_synchronizer::sync_new(const path_info& src, const path_info& dst) {
        if (src.type == fs::file_type::directory) {
            sync_dirs();
        } else if (item.mode != sync_mode::UNSPECIFIED) {
            copy_file_with_timestamp(src, dst);
            sync.state.set(item.path, true, true);
        } else {
            sync_skip();
        }
    }

    void entry_synchronizer::sync_other(const path_info& src, const path_info& dst) {
        log(DEBUG) << "using " << src.name << " because " << format_date_time(src.time) << " > " << format_date_time(dst.time);
        if (src.type == fs::file_type::directory || (dst.type == fs::file_type::directory && item.mode != sync_mode::UNSPECIFIED)) {
            log(INFO) << "deleting " << dst.name << " " << dst.type;
            fs::remove_all(dst.path);
        }
        sync_new(src, dst);
    }

    bool entry_synchronizer::create_dir_if_not_exists(const path_info& p) {
        if (p.type != fs::file_type::not_found) {
            return false;
        }
        log(INFO) << "creating " << p.name << " directory";
        return fs::create_directories(p.path);
    }

    bool entry_synchronizer::delete_if_empty(const path_info& p) {
        if (p.type == fs::file_type::not_found) {
            return true;
        }
        if (!fs::is_empty(p.path)) {
            return false;
        }
        log(INFO) << "deleting empty " << p.name << " " << p.type;
        fs::remove(p.path);
        return true;
    }

    void entry_synchronizer::sync_dirs() {
        if (item.path.empty() || item.mode != sync_mode::UNSPECIFIED) {
            create_dir_if_not_exists(local);
            create_dir_if_not_exists(remote);
            log(TRACE) << "syncing directory";
        } else {
            sync.queue.push_back(sync_item { item.path, item.mode, true });
            log(TRACE) << "processing directory";
        }
        
        queue_map map;
        add_state_names(map);
        if (item.mode != sync_mode::UNSPECIFIED) {
            add_dir_entries(local, map);
            add_dir_entries(remote, map);
        }
        add_index_names(map);

        sync.queue.resize(sync.queue.size() + map.size());

        auto q_it = sync.queue.rbegin();
        for (auto entry: map) {
            *q_it = std::move(entry.second); 
            ++q_it;
        }
        sync.state.set(item.path, item.mode != sync_mode::UNSPECIFIED);
    }

    void entry_synchronizer::sync_skip() {
        log(DEBUG) << "skipping as its not a directory and sync mode is unspecified";
        sync.state.remove(item.path);
    }

    void entry_synchronizer::add_dir_entries(const path_info& i, queue_map& dest) {
        if (i.type != fs::file_type::directory) {
            return;
        }
        for (auto p: fs::directory_iterator(i.path)) {
            if (p.is_directory() && item.mode != sync_mode::RECURSIVE) {
                continue;
            }
            add_queue_map_item(dest, p.path().filename().string(), item.mode);
        }
    }

    void entry_synchronizer::add_state_names(queue_map& dest) {
        const state::node_type* state_node = sync.state.node(item.path);
        if (state_node == nullptr) {
            return;
        }
        for (const auto& state_pair: state_node->entries) {
            add_queue_map_item(dest, state_pair.first, sync_mode::UNSPECIFIED);
        }
    }

    void entry_synchronizer::add_index_names(queue_map& dest) {
        const index::node_type* index_node = sync.index.node(item.path);
        if (index_node == nullptr) {
            return;
        }
        for (const auto& index_pair: index_node->entries) {
            add_queue_map_item(dest, index_pair.first, index_pair.second.data);
        }
    }

    void entry_synchronizer::add_queue_map_item(queue_map& dest, const std::string& name, sync_mode mode) {
        dest[name] = { item.path / name, item.mode == sync_mode::RECURSIVE ? item.mode : mode, false };
    }

    void entry_synchronizer::copy_file_with_timestamp(const path_info& src, const path_info& dst) {
        if (create_parent_dirs(dst.path)) {
            log(INFO) << "created " << dst.name << " parent directories";
        }
        log(INFO) << "copying file from " << src.name << " to " << dst.name;
        lf::copy_file_with_timestamp(src.path, dst.path);
    }

    std::ostream& entry_synchronizer::log(log_level level) {
        max_level = std::max(level, max_level);
        if (!out.view().empty()) {
            out << ", ";
        }
        return out;
    }

}