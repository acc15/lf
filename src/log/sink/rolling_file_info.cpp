#include "log/sink/rolling_file_info.hpp"
#include "io/format_stream.hpp"

#include <charconv>

namespace lf {

    std::filesystem::path rolling_file_info::make_path(const std::filesystem::path& base_file) const {
        const std::string full_name = format_stream() << base_file.filename().string() << name << seq;
        return base_file.parent_path() / full_name;
    }

    std::optional<rolling_file_info> rolling_file_info::parse(std::string_view str, std::string_view prefix) {
        if (str.length() <= prefix.length() || !str.starts_with(prefix)) {
            return std::nullopt;
        }

        str.remove_prefix(prefix.length());

        using sv = std::string_view;

        sv::size_type last_dot_pos = str.find_last_of('.');
        if (last_dot_pos == sv::npos) {
            return std::nullopt;
        }

        sv seq_str = str.substr(last_dot_pos + 1);
        str.remove_suffix(seq_str.length());

        std::size_t seq;
        auto pr = std::from_chars(seq_str.begin(), seq_str.end(), seq);
        if (pr.ec != std::errc {}) {
            return std::nullopt;
        }

        return rolling_file_info { .time = std::filesystem::file_time_type {}, .name = static_cast<std::string>(str), .seq = seq };
    }


}