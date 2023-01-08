#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>
#include <span>

namespace lf {

    class opts {
    public:
        opts& flag();
        opts& default_key(const std::string_view key);

        std::unordered_map<std::string, std::vector<std::string>> parse(const std::span<const char*> args) const;
    };

}