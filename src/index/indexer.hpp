#pragma once

#include <filesystem>
#include <string_view>
#include <optional>

#include "config/config.hpp"
#include "fs/adv_fstream.hpp"
#include "index/index.hpp"

namespace lf {

    class indexer {
    public:
        struct path_info {
            std::filesystem::path abs;
            std::filesystem::path index;
        };

        typedef void (*path_callback) (indexer& indexer, const path_info& info);

        indexer();

        void move(const std::filesystem::path& from, const std::filesystem::path& to);
        void set(const std::filesystem::path& path, sync_mode mode);
        void remove(const std::filesystem::path& path);
        void set_batch(const std::vector<std::string_view>& paths, sync_mode mode);

        std::optional<indexer::path_info> resolve(std::string_view path_str);
        
        bool save_changes();

    private:

        bool success = true;
        tracked_index index;
        adv_fstream file;
        lf::config config;
        
        void load_index();
        void save_index();

    };

}