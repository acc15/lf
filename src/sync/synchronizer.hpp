#pragma once

#include "index/index.hpp"
#include "state/state.hpp"
#include "config/config.hpp"

#include <vector>
#include <unordered_map>
#include <string_view>
#include <utility>

namespace lf {

    class synchronizer {
    public:
        synchronizer(const std::string& name, const config::sync& sync);

        void run();
        void load();
        void save();

        tracked_index index;
        tracked_state state;

    private:

        struct path_info {
            std::string_view name;
            std::filesystem::path path;
            std::filesystem::file_status status;
            std::filesystem::file_type type;
            path_info(std::string_view name, const std::filesystem::path& path);
        };

        struct queue_item {
            std::filesystem::path path;
            sync_mode mode;
        };

        using sync_mode_map = std::unordered_map<std::string, sync_mode>;

        const std::string& name;
        const config::sync& sync;

        std::vector<queue_item> queue;

        void handle(const queue_item& item, const path_info& local, const path_info& remote);
        void handle_dirs(const queue_item& item, const path_info& local, const path_info& remote);
        void handle_not_found(const queue_item& item, const path_info& src, const path_info& dst);
        void handle_other(const queue_item& item, const path_info& src, const path_info& dst);
        void handle_new(const queue_item& item, const path_info& src, const path_info& dst);
        void handle_skip(const queue_item& item);
        void handle_same_time(const queue_item& item, const path_info& local, const path_info& remote, const std::filesystem::file_time_type& time);
        
        void copy_file_with_timestamp(const queue_item& item, const path_info& src, const path_info& dst) const;
        
        void queue_dir_entries(const queue_item& item, const std::filesystem::path* dir_1, const std::filesystem::path* dir_2 = nullptr);
        void add_dir_names(const queue_item& item, const std::filesystem::path* dir_path, sync_mode_map& dest) const;
        void add_state_names(const queue_item& item, sync_mode_map& dest) const;
        void add_index_names(const queue_item& item, sync_mode_map& dest) const;

    };

}