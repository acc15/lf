#pragma once

#include "config/config.hpp"

#include <span>
#include <unordered_map>
#include <filesystem>
#include <utility>
#include <string_view>

#include "index/index_tree.hpp"
#include "index/sync_mode.hpp"

namespace lf {

    class indexer {
    public:
        bool process(const std::span<const char*> paths, sync_mode mode);

        bool process_path(const config& cfg, std::string_view path_str, sync_mode mode);
        bool save_changes() const;

    private:
        using index_change = std::pair<index_tree, bool>;
        using index_map = std::unordered_map<std::filesystem::path, index_change>;

        index_map _indexes;

        void set_index_mode(const config::sync& sync, const std::filesystem::path& rel_path, sync_mode mode);
        index_change& load_index(const std::filesystem::path& index_path);
        
    };

}