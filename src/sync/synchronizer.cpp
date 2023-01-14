#include "sync/synchronizer.hpp"
#include "io/log.hpp"
#include "io/datetime.hpp"
#include "io/file_type.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace lf {

    synchronizer::path_info::path_info(std::string_view name, const std::filesystem::path& path): 
        name(name),
        path(path), 
        status(fs::status(path)), 
        type(status.type()) 
    {
    }

    synchronizer::synchronizer(const std::string& name, const config::sync& sync): name(name), sync(sync) {
    }

    void synchronizer::run() {
        queue = { queue_item { .path = fs::path(), .mode = index.get() } };
        while (!queue.empty()) {
            
            const queue_item item = std::move(queue.back());
            queue.pop_back();

            const path_info li = { "local", item.path.empty() ? sync.local : sync.local / item.path };
            const path_info ri = { "remote", item.path.empty() ? sync.remote : sync.remote / item.path };
            handle(item, li, ri);

        }
    }

    void synchronizer::handle(const queue_item& item, const path_info& local, const path_info& remote) {
        
        if (local.type == fs::file_type::not_found && remote.type == fs::file_type::not_found) {
            log.info() && log() << item.path << ": no file or directory exists on local and remote sides" << std::endl;
            index.remove(item.path);
            state.remove(item.path);
            return;
        }

        if (item.mode == sync_mode::IGNORE) {
            log.info() && log() << item.path << ": ignored, skipping..." << std::endl;
            state.remove(item.path);
            return;
        }

        if (remote.type == fs::file_type::not_found) {
            handle_not_found(item, local, remote);
            return;
        }
        
        if (local.type == fs::file_type::not_found) {
            handle_not_found(item, remote, local);
            return;
        }

        if (local.type == fs::file_type::directory && remote.type == fs::file_type::directory) {
            handle_dirs(item, local, remote);
            return;
        }

        const fs::file_time_type local_time = fs::last_write_time(local.path);
        const fs::file_time_type remote_time = fs::last_write_time(remote.path);
        if (local_time == remote_time) {
            handle_same_time(item, local, remote, local_time);
        } else if (local_time > remote_time) {
            handle_other(item, local, remote);
        } else {
            handle_other(item, remote, local);
        }

    }

    void synchronizer::handle_same_time(const queue_item& item, const path_info& local, const path_info& remote, const fs::file_time_type& time) {
        if (item.mode == sync_mode::UNSPECIFIED) {
            handle_skip(item);
            return;
        }

        if (local.type == remote.type) {
            log.info() && log() << item.path << ": synced, both entries has same modification time (" << format_date_time(time) << ") and same type: " << local.type << std::endl;
            state.set(item.path, true, true);
            return;
        }

        log.warn() && log() << item.path << ": CONFLICT, both entries has same modification time (" << format_date_time(time) 
            << "), but different types, local is " << local.type << ", remote is " << remote.type << std::endl;
        state.remove(item.path);
    }

    void synchronizer::handle_dirs(const queue_item& item, const path_info& src, const path_info& dst) {
        log.info() && log() << item.path << ": both are directories, processing children" << std::endl;
        add_to_queue(item, &src.path, &dst.path);
    }

    void synchronizer::handle_not_found(const queue_item& item, const path_info& src, const path_info& dst) {
        if (state.get(item.path)) {
            log.info() && log() << item.path << ": was synced, but removed in " << dst.name << ", deleting in " << src.name << std::endl;
            fs::remove_all(src.path);
            state.remove(item.path);
            index.remove(item.path);
        } else {
            handle_new(item, src, dst);
        }
    }

    void synchronizer::handle_other(const queue_item& item, const path_info& src, const path_info& dst) {
        if (src.type == fs::file_type::directory || (dst.type == fs::file_type::directory && item.mode != sync_mode::UNSPECIFIED)) {
            log.info() && log() << item.path << ": deleting " << dst.name << " " << dst.type << std::endl;
            fs::remove_all(dst.path);
        }
        handle_new(item, src, dst);
    }

    void synchronizer::handle_new(const queue_item& item, const path_info& src, const path_info& dst) {
        if (src.type == fs::file_type::directory) {
            
            log.info() && log() << item.path << ": creating directory in " << dst.name << std::endl;
            fs::create_directory(dst.path, src.path);
            add_to_queue(item, &src.path, nullptr);

        } else if (item.mode != sync_mode::UNSPECIFIED) {
            
            copy_file_with_timestamp(item, src, dst);
            state.set(item.path, true, true);

        } else {
            
            handle_skip(item);

        }
    }

    void synchronizer::add_to_queue(const queue_item& item, const std::filesystem::path* dir_1, const std::filesystem::path* dir_2) {
        
        sync_mode_map map;

        add_state_names(item, map);
        add_dir_names(dir_1, item, map);
        add_dir_names(dir_2, item, map);
        add_index_names(item, map);
        
        for (const auto& child_pair: map) {
            queue.push_back(queue_item { item.path / child_pair.first, child_pair.second });
        }

        state.set(item.path, item.mode != sync_mode::UNSPECIFIED);

    }

    void synchronizer::handle_skip(const queue_item& item) {
        log.info() && log() << item.path << ": skipping as its not a directory and sync mode is unspecified" << std::endl;
        state.remove(item.path);
    }

    void synchronizer::add_dir_names(const std::filesystem::path* dir_path, const queue_item& item, sync_mode_map& dest) const {
        if (dir_path == nullptr || item.mode == sync_mode::UNSPECIFIED) {
            return;
        }
        for (auto p: fs::directory_iterator(*dir_path)) {
            if (p.is_directory() && item.mode != sync_mode::RECURSIVE) {
                continue;
            }
            
            const fs::path child_path = p.path();
            const fs::path child_filename = child_path.filename();
            const std::string child_name = child_filename.string();
            
            dest[child_name] = item.mode;
        }
    }

    void synchronizer::add_state_names(const queue_item& item, sync_mode_map& dest) const {
        const state::node_type* state_node = state.node(item.path);
        if (state_node == nullptr) {
            return;
        }
        const sync_mode state_mode = item.mode == sync_mode::RECURSIVE ? sync_mode::RECURSIVE : sync_mode::UNSPECIFIED;
        for (const auto& state_pair: state_node->entries) {
            dest[state_pair.first] = state_mode;
        }
    }

    void synchronizer::add_index_names(const queue_item& item, sync_mode_map& dest) const {
        const index::node_type* index_node = index.node(item.path);
        if (index_node == nullptr) {
            return;
        }
        for (const auto& index_pair: index_node->entries) {
            dest[index_pair.first] = item.mode == sync_mode::RECURSIVE ? sync_mode::RECURSIVE : index_pair.second.data;
        }
    }

    void synchronizer::copy_file_with_timestamp(const queue_item& item, const path_info& src, const path_info& dst) const {
        log.info() && log() << item.path << ": copying file from " << src.name << " to " << dst.name << std::endl;
        fs::copy_file(src.path, dst.path, fs::copy_options::overwrite_existing);
        fs::file_time_type src_time = fs::last_write_time(src.path);
        fs::last_write_time(dst.path, src_time);
    }

    void synchronizer::load() {
        try {
            index.load(sync.index);
        } catch (const file_not_found_error& e) {
            log.warn() && log() << e.what() << std::endl;
        }
        try {
            state.load(sync.state);
        } catch (const file_not_found_error& e) {
            log.debug() && log() << e.what() << std::endl;
        }
    }

    void synchronizer::save() {
        index.save_if_changed(sync.index);
        state.save_if_changed(sync.state);
    }

}