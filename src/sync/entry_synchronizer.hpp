#pragma once

#include <sstream>
#include <map>
#include "sync/sync_item.hpp"
#include "sync/path_info.hpp"
#include "sync/synchronizer.hpp"
#include "log/log_level.hpp"

namespace lf {

    class entry_synchronizer {

        synchronizer& s;
        const sync_item& item;
        path_info local;
        path_info remote;

        log_level max_level;
        std::stringstream out;

    public:

        entry_synchronizer(synchronizer& sync, const sync_item& item);

        void run();

        const std::filesystem::path& path() const;
        std::string message() const;
        log_level level() const;

        bool create_dir_if_not_exists(const path_info& p);

    private:
        using queue_map = std::map<std::string, sync_item>;

        void cleanup();

        void process();
        void process_dir();

        void sync();
        void sync_not_found(const path_info& src, const path_info& dst);
        void sync_del(const path_info& src, const path_info& dst);
        void sync_new(const path_info& src, const path_info& dst);
        void sync_dirs();
        void sync_with_timestamps();
        void sync_same_time();
        void sync_other(const path_info& src, const path_info& dst);

        void add_dir_entries(const path_info& i, queue_map& dest);
        void add_state_names(queue_map& dest);
        void add_index_names(queue_map& dest);
        void add_queue_map_item(queue_map& dest, const std::string& name, sync_mode mode);

        void queue(const queue_map& map);
        void copy_file(const path_info& src, const path_info& dst);

        bool delete_empty_dir(const path_info& p);
        bool delete_if_exists(const path_info& p);
        
        std::ostream& log(log_level level);

    };

}