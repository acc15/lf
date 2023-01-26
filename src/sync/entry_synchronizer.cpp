#include "sync/entry_synchronizer.hpp"
#include "fs/time.hpp"

#include "io/time.hpp"
#include "io/file_type.hpp"
#include "fs/time.hpp"
#include "fs/util.hpp"

namespace fs = std::filesystem;

namespace lf {

    entry_synchronizer::entry_synchronizer(synchronizer& sync, std::ostream& out):
        out(out),
        sync(sync),
        item(std::move(sync.queue.back())),
        local(true, join_path(sync.config.local, item.path)),
        remote(false, join_path(sync.config.remote, item.path))
    {
        sync.queue.pop_back();
    }

    void entry_synchronizer::run() {
        out << (item.path.empty() ? "<root>" : item.path.string()) << ": ";
        if (item.mode == sync_mode::IGNORE) {
            out << "ignored, skipping...";
            sync.state.remove(item.path);
        } else if (local.type == fs::file_type::not_found && remote.type == fs::file_type::not_found) {
            out << "no file or directory exists on local and remote sides";
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

    void entry_synchronizer::sync_with_timestamps() {
        const fs::file_time_type local_time = ntfs_last_write_time(local.path);
        const fs::file_time_type remote_time = ntfs_last_write_time(remote.path);
        if (local_time == remote_time) {
            sync_same_time(local_time);
        } else if (local_time > remote_time) {
            out << "using local because " << format_date_time(local_time) << " > " << format_date_time(remote_time) << ", ";
            sync_other(local, remote);
        } else {
            out << "using remote because " << format_date_time(local_time) << " < " << format_date_time(remote_time) << ", ";
            sync_other(remote, local);
        }
    }

    void entry_synchronizer::sync_same_time(const fs::file_time_type& time) {
        if (item.mode == sync_mode::UNSPECIFIED) {
            sync_skip();
        } else if (local.type == remote.type) {
            out << "synced, both entries has same modification time (" << format_date_time(time) << ") and same type: " << local.type;
            sync.state.set(item.path, true, true);
        } else {
            out << "CONFLICT, both entries has same modification time (" << format_date_time(time) 
                << "), but different types, local is " << local.type << ", remote is " << remote.type;
            sync.state.remove(item.path);
        }
    }

    void entry_synchronizer::sync_dirs() {
        out << (item.method == sync_method::SYNC ? "syncing directory" : "finalizing directory");
        queue_dir_entries({ &local.path, &remote.path });
    }

    void entry_synchronizer::sync_not_found(const path_info& src, const path_info& dst) {
        if (sync.state.get(item.path)) {
            out << "was deleted in " << dst.name << ", deleting in " << src.name;
            fs::remove_all(src.path);
            sync.state.remove(item.path);
            sync.index.remove(item.path);
        } else {
            sync_new(src, dst);
        }
    }

    void entry_synchronizer::sync_other(const path_info& src, const path_info& dst) {
        if (src.type == fs::file_type::directory || (dst.type == fs::file_type::directory && item.mode != sync_mode::UNSPECIFIED)) {
            out << "deleting " << dst.name << " " << dst.type << ", ";
            fs::remove_all(dst.path);
        }
        sync_new(src, dst);
    }

    void entry_synchronizer::sync_new(const path_info& src, const path_info& dst) {
        if (src.type == fs::file_type::directory) {
            sync_new_dir(src, dst);
        } else if (item.mode != sync_mode::UNSPECIFIED) {
            copy_file_with_timestamp(src, dst);
            sync.state.set(item.path, true, true);
        } else {
            sync_skip();
        }
    }

    void entry_synchronizer::sync_new_dir(const path_info& src, const path_info& dst) {
        if (item.mode != sync_mode::UNSPECIFIED) {
            out << "creating directory in " << dst.name;
            fs::create_directories(dst.path);
        } else if (item.method != sync_method::SYNC && fs::is_empty(src.path)) {
            out << "deleting empty directory in " << src.name;
            fs::remove(src.path);
            return;
        } else if (item.method == sync_method::POST_FINALIZE) {
            out << src.name << "directory finalized";
            return;
        } else if (item.method == sync_method::FINALIZE) {
            out << "finalizing " << src.name << " directory";
            sync.queue.push_back(sync_item { item.path, item.mode, sync_method::POST_FINALIZE });
        } else {
            out << "syncing " << src.name << " to " << dst.name << " directory";
        }
        queue_dir_entries({ &src.path });
    }

    void entry_synchronizer::queue_dir_entries(const std::initializer_list<const fs::path*>& dirs) {
        queue_map map;

        add_state_names(map);
        for (const fs::path* dir: dirs) {
            add_dir_names(*dir, map);
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
        out << (item.method == sync_method::SYNC ? "skipping as its not a directory and sync mode is unspecified" : "finalized");
        sync.state.remove(item.path);
    }

    void entry_synchronizer::add_dir_names(const std::filesystem::path& dir_path, queue_map& dest) const {
        if (item.mode == sync_mode::UNSPECIFIED) {
            return;
        }
        for (auto p: fs::directory_iterator(dir_path)) {
            if (p.is_directory() && item.mode != sync_mode::RECURSIVE) {
                continue;
            }
            
            const fs::path child_path = p.path();
            const fs::path child_filename = child_path.filename();
            const std::string child_name = child_filename.string();
            
            dest[child_name] = { item.path / child_filename, item.mode, sync_method::SYNC };
        }
    }

    void entry_synchronizer::add_state_names(queue_map& dest) const {
        const state::node_type* state_node = sync.state.node(item.path);
        if (state_node == nullptr) {
            return;
        }
        const sync_mode state_mode = item.mode == sync_mode::RECURSIVE ? sync_mode::RECURSIVE : sync_mode::UNSPECIFIED;
        for (const auto& state_pair: state_node->entries) {
            dest[state_pair.first] = { item.path / state_pair.first, state_mode, sync_method::FINALIZE };
        }
    }

    void entry_synchronizer::add_index_names(queue_map& dest) const {
        const index::node_type* index_node = sync.index.node(item.path);
        if (index_node == nullptr) {
            return;
        }
        for (const auto& index_pair: index_node->entries) {
            dest[index_pair.first] = { 
                item.path / index_pair.first, 
                item.mode == sync_mode::RECURSIVE ? sync_mode::RECURSIVE : index_pair.second.data, 
                sync_method::SYNC
            };
        }
    }

    void entry_synchronizer::copy_file_with_timestamp(const path_info& src, const path_info& dst) const {
        if (dst.path.has_parent_path()) {
            const fs::path dst_dir = dst.path.parent_path();
            if (!fs::exists(dst.path.parent_path())) {
                out << "creating " << dst.name << " directory " << dst_dir << ", ";
                fs::create_directories(dst_dir);
            }
        }

        out << "copying file from " << src.name << " to " << dst.name;
        lf::copy_file_with_timestamp(src.path, dst.path);
    }

}