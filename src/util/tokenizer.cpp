#include <cassert>
#include <algorithm>

#include "tokenizer.hpp"

namespace leafsync {


    bool tokenizer::next(std::string_view& result, std::string_view separator) {
        if (separator.empty()) {
            result = chunk;
            chunk.remove_prefix(chunk.size());
            return true;
        }

        const auto size = chunk.find(separator);
        if (size == std::string_view::npos) {
            return false;
        }

        result = chunk.substr(0, size);
        chunk.remove_prefix(size + separator.size());
        return true;
    }

    bool tokenizer::parse(const std::span<std::string_view>& tokens, const std::span<const std::string_view>& separators) {
        size_t sz = tokens.size();
        assert(sz == separators.size());
        for (size_t i = 0; i < sz; i++) {
            if (!next(tokens[i], separators[i])) {
                return false;
            }
        }
        return true;
    }

}