#pragma once

#include <istream>
#include <concepts>
#include <string_view>

#include "util/string.hpp"
#include "log/log.hpp"
#include "io/util.hpp"

namespace lf {

    struct config_entry {
        std::size_t line;
        std::string section;
        std::string_view key;
        std::string_view value;
    };

    template <std::invocable<const config_entry&> Callback>
    std::istream& parse_config(std::istream& s, Callback callback) {

        config_entry entry = { .line = 0, .section {}, .key {}, .value {} };
        
        std::string line;
        while (std::getline(s, line)) {
            ++entry.line;

            std::string_view sv = trim(line);
            if (sv.empty() || sv.starts_with("#")) {
                continue;
            }

            if (sv.starts_with('[') && sv.ends_with(']')) {
                sv.remove_prefix(1);
                sv.remove_suffix(1);
                entry.section = static_cast<std::string>(sv);
                continue;
            }

            std::string_view::size_type eq_pos = sv.find('=');
            if (eq_pos == std::string_view::npos) {
                log.error() && log() << "invalid input line at " << entry.line << ": " << line << log::end;
                continue;
            }

            entry.key = rtrim(sv.substr(0, eq_pos));
            if (entry.key.empty()) {
                log.error() && log() << "empty key at " << entry.line << ": " << line << log::end;
                continue;
            }

            entry.value = ltrim(sv.substr(eq_pos + 1));
            callback(entry); 
        }
        return reset_fail_on_eof(s);
        
    }
}
