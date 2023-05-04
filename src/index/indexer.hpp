#pragma once

#include "config/config.hpp"
#include "fs/adv_fstream.hpp"

#include <span>
#include <filesystem>
#include <string_view>
#include <optional>

#include "index/index.hpp"
#include "index/sync_mode.hpp"

namespace lf {

    class indexer {
    public:
        indexer();

        void process(const std::vector<std::string_view>& paths, std::optional<sync_mode> mode);
        void process(std::string_view path_str, std::optional<sync_mode> mode);
        bool save_changes();

    private:

        bool _success = true;
        tracked_index _index;
        adv_fstream _file;
        config _config;

        bool validate(sync_mode mode, const std::filesystem::path& path) const;
        tracked_index& load_index(const std::filesystem::path& index_path);
        
        void init_index();
        void save_index();

    };

}