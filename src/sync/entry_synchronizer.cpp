#include "sync/entry_synchronizer.hpp"

#include "io/time.hpp"
#include "io/file_type.hpp"
#include "fs/util.hpp"

namespace fs = std::filesystem;

using fs::file_type::not_found;
using fs::file_type::directory;

namespace lf {

    entry_synchronizer::entry_synchronizer(synchronizer& sync, const sync_item& item):
        s(sync),
        item(item),
        local(true, sync.config.local, item.path),
        remote(false, sync.config.remote, item.path),
        max_level(TRACE)
    {
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
            process();
        } else {
            sync();
        }
    }

    void entry_synchronizer::cleanup() {
        log(TRACE) << "cleanup directory";
        s.state.remove(item.path, true);
        if (!item.path.empty()) {
            delete_empty_dir(local);
            delete_empty_dir(remote);
        }
    }

    void entry_synchronizer::process() {
        if (remote.type == directory) {
            process_dir();
            return;
        }
        if (remote.type != not_found) {
            log(INFO) << "deleting remote " << remote.type;
            fs::remove(remote.path);
        }
        if (local.type == directory) {
            process_dir();
        } else {
            log(DEBUG) << "ignored";
            s.state.remove(item.path);
        }
    }

    void entry_synchronizer::process_dir() {        
        queue_map map;
        add_state_names(map);
        add_index_names(map);
        if (map.empty()) {
            cleanup();
            return;
        }

        log(TRACE) << "processing directory";
        s.state.set(item.path, false);
        s.queue.push_back(sync_item { item.path, item.mode, true });
        queue(map);

    }

    void entry_synchronizer::sync() {
        if (local.type == not_found && remote.type == not_found) {
            log(INFO) << "no file or directory exists on both sides";
            s.state.remove(item.path);
        } else if (remote.type == not_found) {
            sync_not_found(local, remote);
        } else if (local.type == not_found) {
            sync_not_found(remote, local);
        } else if (local.type == directory && remote.type == directory) {
            sync_dirs();
        } else {
            sync_with_timestamps();
        }
    }

    void entry_synchronizer::sync_not_found(const path_info& src, const path_info& dst) {
        if (s.state.get(item.path)) {
            sync_del(src, dst);
        } else {
            sync_new(src, dst);
        }
    }

    void entry_synchronizer::sync_del(const path_info& src, const path_info& dst) {
        log(INFO) << "was deleted in " << dst.name << ", deleting in " << src.name;
        fs::remove_all(src.path);
        s.state.remove(item.path);
    }

    void entry_synchronizer::sync_new(const path_info& src, const path_info& dst) {
        if (src.type == fs::file_type::directory) {
            sync_dirs();
        } else {
            copy_file(src, dst);
        }
    }

    void entry_synchronizer::sync_dirs() {
        log(TRACE) << "syncing directory";
        create_dir_if_not_exists(local);
        create_dir_if_not_exists(remote);

        queue_map map;
        add_state_names(map);
        add_dir_entries(local, map);
        add_dir_entries(remote, map);
        add_index_names(map);

        queue(map);
        s.state.set(item.path, true);
    }

    void entry_synchronizer::sync_with_timestamps() {
        local.init_time();
        remote.init_time();
        if (local.time == remote.time) {
            sync_same_time();
        } else if (local.time > remote.time) {
            sync_other(local, remote);
        } else {
            sync_other(remote, local);
        }
    }

    void entry_synchronizer::sync_same_time() {
        if (local.type == remote.type) {
            log(DEBUG) << "synced, both entries has same modification time (" << format_date_time(local.time) << ") and same type: " << local.type;
            s.state.set(item.path, true, true);
        } else {
            log(WARN) << "CONFLICT, both entries has same modification time (" << format_date_time(local.time) 
                << "), but different types, local is " << local.type << ", remote is " << remote.type;
            s.state.remove(item.path);
        }
    }

    void entry_synchronizer::sync_other(const path_info& src, const path_info& dst) {
        log(DEBUG) << "using " << src.name << " because " << format_date_time(src.time) << " > " << format_date_time(dst.time);
        if (src.type == fs::file_type::directory || dst.type == fs::file_type::directory) {
            log(INFO) << "deleting " << dst.name << " " << dst.type;
            fs::remove_all(dst.path);
        }
        sync_new(src, dst);
    }

    bool entry_synchronizer::create_dir_if_not_exists(const path_info& p) {
        if (p.type == fs::file_type::directory) {
            return false;
        }

        const fs::path::iterator b = p.item.begin();
        const fs::path::iterator e = p.item.end();
        fs::path::iterator it = e;

        path_info cur = p;
        for (; cur.type != fs::file_type::directory && it != b; --it, cur = cur.parent()) {
            if (cur.type != fs::file_type::not_found) {
                log(INFO) << "deleted " << cur.name << " " << cur.type << " " << cur.item;
                fs::remove(cur.path);
            }
        }

        fs::path path = cur.path;
        for (; it != e; ++it) {
            path /= *it;
            if (!fs::create_directory(path)) {
                throw fs::filesystem_error("unable to create directory", path, std::error_code {});
            }
        }

        log(INFO) << "created " << p.name << " directory " << p.item;
        return true;
    }

    bool entry_synchronizer::delete_empty_dir(const path_info& p) {
        if (p.type != fs::file_type::directory || !fs::is_empty(p.path)) {
            return false;
        }
        log(INFO) << "deleting empty " << p.name << " " << p.type;
        fs::remove(p.path);
        return true;
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
        const state::node_type* state_node = s.state.node(item.path);
        if (state_node == nullptr) {
            return;
        }
        for (const auto& state_pair: state_node->entries) {
            add_queue_map_item(dest, state_pair.first, sync_mode::IGNORE);
        }
    }

    void entry_synchronizer::add_index_names(queue_map& dest) {
        const index::node_type* index_node = s.index.node(item.path);
        if (index_node == nullptr) {
            return;
        }
        for (const auto& index_pair: index_node->entries) {
            add_queue_map_item(dest, index_pair.first, index_pair.second.data);
        }
    }

    void entry_synchronizer::add_queue_map_item(queue_map& dest, const std::string& name, sync_mode mode) {
        dest[name] = { item.path / name, mode, false };
    }

    void entry_synchronizer::queue(queue_map& map) {
        s.queue.resize(s.queue.size() + map.size());
        auto q_it = s.queue.rbegin();
        for (auto entry: map) {
            *q_it = std::move(entry.second); 
            ++q_it;
        }
    }

    void entry_synchronizer::copy_file(const path_info& src, const path_info& dst) {
        create_dir_if_not_exists(dst.parent());
        log(INFO) << "copying file from " << src.name << " to " << dst.name;
        copy_file_with_timestamp(src.path, dst.path);
        s.state.set(item.path, true, true);
    }

    std::ostream& entry_synchronizer::log(log_level level) {
        max_level = std::max(level, max_level);
        if (!out.view().empty()) {
            out << ", ";
        }
        return out;
    }

}