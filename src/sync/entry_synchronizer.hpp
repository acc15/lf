#pragma once

#include <ostream>
#include <map>
#include "sync/sync_item.hpp"
#include "sync/path_info.hpp"
#include "sync/synchronizer.hpp"

namespace lf {

    class entry_synchronizer {

        std::ostream& out;
        synchronizer& sync;
        sync_item item;
        path_info local;
        path_info remote;

    public:

        entry_synchronizer(synchronizer& sync, std::ostream& out);

        void run();

    private:
        using queue_map = std::map<std::string, sync_item>;

        void cleanup() const;

        void sync_with_timestamps();
        void sync_dirs();
        void sync_not_found(const path_info& src, const path_info& dst);
        void sync_other(const path_info& src, const path_info& dst);

        void sync_del(const path_info& src, const path_info& dst);
        void sync_new(const path_info& src, const path_info& dst);
        void sync_new_dir(const path_info& src, const path_info& dst);

        void sync_skip();
        void sync_same_time();
        
        void add_dir_entries(const path_info& i, queue_map& dest) const;
        void add_state_names(queue_map& dest) const;
        void add_index_names(queue_map& dest) const;
        void add_queue_map_item(queue_map& dest, const std::string& name, sync_mode mode) const;

        void copy_file_with_timestamp(const path_info& src, const path_info& dst) const;

        bool create_dir_if_not_exists(const path_info& p) const;
        bool delete_if_empty(const path_info& p) const;
        
    };

}