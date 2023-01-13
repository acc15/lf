#pragma once

#include "index/index.hpp"
#include "state/state.hpp"
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

        struct queue_item {
            std::filesystem::path path;
            sync_mode mode;
        };

        synchronizer(const std::string& name, const config::sync& sync, bool dry);

        const std::string& name;
        const config::sync& sync;

        bool dry;
        tracked_index index;
        tracked_state state;

        std::vector<queue_item> queue;

        void run();
        void load();
        void save();

    private:

        void handle(const queue_item& item, const path_info& l, const path_info& r);

    };

}