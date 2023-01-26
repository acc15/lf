#pragma once

#include "config/config.hpp"

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
        void process(const config& cfg, const std::vector<std::string_view>& paths, std::optional<sync_mode> mode);
        void process(const config& cfg, std::string_view path_str, std::optional<sync_mode> mode);
        void save_changes();

        bool is_successful() const;

    private:
        using index_map = std::unordered_map<std::filesystem::path, tracked_index>;

        bool success = true;
        index_map _indexes;

        bool validate(sync_mode mode, const std::filesystem::path& path) const;
        tracked_index& load_index(const std::filesystem::path& index_path);
        
    };

}