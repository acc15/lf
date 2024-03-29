#include "sync/entry_synchronizer.hpp"

#include "io/time.hpp"
#include "fs/file_type.hpp"
#include "fs/util.hpp"

namespace fs = std::filesystem;

using fs::file_type::not_found;
using fs::file_type::directory;

namespace lf {

    entry_synchronizer::entry_synchronizer(synchronizer& sync, const sync_item& item):
        s(sync),
        item(item),
        left(true, sync.config.left, item.path),
        right(false, sync.config.right, item.path),
        max_log_level(TRACE)
    {
    }

    const std::filesystem::path& entry_synchronizer::path() const {
        return item.path;
    }

    std::string entry_synchronizer::message() const {
        return out.str();
    }

    log_level entry_synchronizer::level() const {
        return max_log_level;
    }

    void entry_synchronizer::run() {
        if (item.cleanup) {
            cleanup();
        } else if (item.mode == sync_mode::NONE || item.mode == sync_mode::IGNORE) {
            process();
        } else {
            sync();
        }
    }

    void entry_synchronizer::cleanup() {
        log(TRACE) << "cleanup directory";
        if (item.path.empty()) {
            return;
        }
        if (item.mode == sync_mode::NONE && (left.type == directory || s.state.get(item.path))) {
            delete_empty_dir_or_file(left);
        }
        delete_empty_dir_or_file(right);
        s.state.remove(item.path, true);
    }

    void entry_synchronizer::process() {
        log(TRACE) << "processing";
        if (right.type == directory || left.type == directory) {
            process_dir();
        } else {
            process_other();
        }
    }

    void entry_synchronizer::process_other() {
        delete_path(right);
        if (!s.state.get(item.path)) {
            log(DEBUG) << "not synced";
            return;
        }
        if (item.mode == sync_mode::IGNORE) {
            log(DEBUG) << "ignored";
        } else {
            delete_path(left);
        }
        s.state.remove(item.path);
    }

    void entry_synchronizer::process_dir() {
        queue_map map;
        add_state_names(map);
        add_index_names(map);
        if (map.empty()) {
            cleanup();
            return;
        }

        s.queue.push_back(sync_item { item.path, item.mode, true });
        queue(map);
    }

    void entry_synchronizer::sync() {
        if (left.type == not_found && right.type == not_found) {
            log(INFO) << "no file or directory exists on both sides";
            s.state.remove(item.path);
        } else if (right.type == not_found) {
            sync_not_found(left, right);
        } else if (left.type == not_found) {
            sync_not_found(right, left);
        } else if (left.type == directory && right.type == directory) {
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
        fs::remove_all(src.full_path);
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
        create_dir_if_not_exists(left);
        create_dir_if_not_exists(right);

        queue_map map;
        add_state_names(map);
        add_dir_entries(left, map);
        add_dir_entries(right, map);
        add_index_names(map);

        queue(map);
        s.state.set(item.path, true);
    }

    void entry_synchronizer::sync_with_timestamps() {
        left.init_time();
        right.init_time();
        if (left.time == right.time) {
            sync_same_time();
        } else if (left.time > right.time) {
            sync_other(left, right);
        } else {
            sync_other(right, left);
        }
    }

    void entry_synchronizer::sync_same_time() {
        if (left.type == right.type) {
            log(DEBUG) << "synced, both entries has same modification time (" << format_date_time(left.time) << ") and same type: " << left.type;
            s.state.set(item.path, true, true);
        } else {
            log(WARN) << "CONFLICT, both entries has same modification time (" << format_date_time(left.time) 
                << "), but different types, left is " << left.type << ", right is " << right.type;
            s.state.remove(item.path);
        }
    }

    void entry_synchronizer::sync_other(const path_info& src, const path_info& dst) {
        log(DEBUG) << "using " << src.name << " because " << format_date_time(src.time) << " > " << format_date_time(dst.time);
        if (src.type == fs::file_type::directory || dst.type == fs::file_type::directory) {
            log(INFO) << "deleting " << dst.name << " " << dst.type;
            fs::remove_all(dst.full_path);
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
                fs::remove(cur.full_path);
            }
        }

        fs::path cur_path = cur.full_path;
        for (; it != e; ++it) {
            cur_path /= *it;
            if (!fs::create_directory(cur_path)) {
                throw fs::filesystem_error("unable to create directory", cur_path, std::error_code {});
            }
        }

        log(INFO) << "created " << p.name << " directory " << p.item;
        return true;
    }

    bool entry_synchronizer::delete_empty_dir_or_file(const path_info& p) {
        if (p.type == not_found || (p.type == directory && !fs::is_empty(p.full_path))) {
            return false;
        }
        delete_path(p);
        return true;
    }

    void entry_synchronizer::delete_path(const path_info& p) {
        if (p.type != not_found) {
            log(INFO) << "deleting " << p.name << " " << p.type;
            fs::remove(p.full_path);
        } else {
            log(TRACE) << p.name << " not exists";
        }
    }

    void entry_synchronizer::add_dir_entries(const path_info& i, queue_map& dest) {
        if (i.type != fs::file_type::directory) {
            return;
        }
        for (auto p: fs::directory_iterator(i.full_path)) {
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
        const sync_mode next_mode = item.mode == sync_mode::IGNORE ? sync_mode::IGNORE : sync_mode::NONE;
        for (const auto& state_pair: state_node->entries) {
            add_queue_map_item(dest, state_pair.first, next_mode);
        }
    }

    void entry_synchronizer::add_index_names(queue_map& dest) {
        const index::node_type* index_node = s.index.node(item.path);
        if (index_node == nullptr) {
            return;
        }
        for (const auto& index_pair: index_node->entries) {
            const sync_mode entry_mode = index_pair.second.data;
            // inheriting RECURSIVE mode when index has NONE (intermediate node)
            const sync_mode effective_mode = (item.mode == sync_mode::IGNORE || item.mode == sync_mode::RECURSIVE) 
                && entry_mode == sync_mode::NONE ? item.mode : entry_mode;
                
            add_queue_map_item(dest, index_pair.first, effective_mode);
        }
    }

    void entry_synchronizer::add_queue_map_item(queue_map& dest, const std::string& name, sync_mode mode) {
        dest[name] = { item.path / name, mode, false };
    }

    void entry_synchronizer::queue(const queue_map& map) {
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
        copy_file_with_timestamp(src.full_path, dst.full_path);
        s.state.set(item.path, true, true);
    }

    std::ostream& entry_synchronizer::log(log_level level) {
        max_log_level = std::max(level, max_log_level);
        if (!out.view().empty()) {
            out << ", ";
        }
        return out;
    }

}