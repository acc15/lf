#pragma once

#include "config/config.hpp"
#include "fs/adv_fstream.hpp"

#include <span>
#include <unordered_map>
#include <filesystem>
#include <utility>
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
        using index_pair = std::pair<tracked_index, adv_fstream>;
        using index_map = std::unordered_map<std::filesystem::path, index_pair>;
        using index_entry = index_map::value_type;

        bool _success = true;
        index_map _indexes;
        config _config;

        bool validate(sync_mode mode, const std::filesystem::path& path) const;
        tracked_index& load_index(const std::filesystem::path& index_path);
        
        void init_entry(const std::filesystem::path& path, tracked_index& index, adv_fstream& file);
        void save_entry(const std::filesystem::path& path, tracked_index& index, adv_fstream& file);

    };

}