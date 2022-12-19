#pragma once

#include <span>
#include <string_view>

namespace leafsync {

    class tokenizer {
    public:
        std::string_view chunk;
        bool next(std::string_view& result, std::string_view separator = "");
        bool parse(const std::span<std::string_view>& tokens, const std::span<const std::string_view>& separators);
    };

}