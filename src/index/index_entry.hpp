#pragma once

#include <cstdint>

#include <string>
#include <filesystem>
#include <unordered_map>
#include <optional>

#include "../io/with_format.hpp"

#include "index_flags.hpp"

namespace lf {

    extern const char index_signature[];
    extern const std::uint8_t index_version;

    struct index_entry {
        
        using entry_map = std::unordered_map<std::string, index_entry>;

        index_flags flags;
        entry_map entries = {};

        index_entry* entry(const std::filesystem::path& path);
        const index_entry* entry(const std::filesystem::path& path) const;

        index_flags get(const std::filesystem::path& path) const;
        void set(const std::filesystem::path& path, index_flags flags);

    private:
        index_entry& create_entry(const std::filesystem::path& path);
        std::pair<index_entry*, const std::string*> compute_removal_pair(const std::filesystem::path& path, index_flags flags);

    };

    std::ostream& operator<<(std::ostream& s, const index_entry& index);
    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const index_entry&> index);
    std::istream& operator>>(std::istream& s, with_format_and_errors<format::BINARY, index_entry&> index);

}


