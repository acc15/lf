#pragma once

#include <filesystem>
#include <string>
#include <optional>

namespace lf {

    struct rolling_file_info {
        std::filesystem::file_time_type time;
        std::string name;
        std::size_t seq;

        auto operator<=>(const rolling_file_info& other) const = default;

        std::filesystem::path make_path(const std::filesystem::path& base_file) const;

        static std::optional<rolling_file_info> parse(std::string_view str, std::string_view prefix);

    };

}