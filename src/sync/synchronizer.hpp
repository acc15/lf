#pragma once

#include "index/index_tree.hpp"
#include "state/state_tree.hpp"
#include "config/config.hpp"

#include <vector>
#include <utility>

namespace lf {

    class synchronizer {
    public:

        struct path_info {
            std::filesystem::path path;
            std::filesystem::file_status status;
            std::filesystem::file_type type;
            path_info(const std::filesystem::path& path);
        };


        synchronizer(const std::string& name, const config::sync& sync);

        const std::string& name;
        const config::sync& sync;

        index_tree index;
        state_tree state;

        std::vector<std::pair<std::filesystem::path, sync_mode>> queue;

        void run();
        bool init();

    private:
        void local_to_remote(const path_info& l, const path_info& r);
        void remote_to_local(const path_info& l, const path_info& r);

    };

}