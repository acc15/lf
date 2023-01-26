#pragma once

#include <span>
#include <vector>
#include <ostream>
#include <string_view>

#include "opts/opt.hpp"
#include "opts/opt_map.hpp"

namespace lf {

    struct opts {
        std::vector<struct opt> opts;
        std::string_view default_opt = "";
        opt_map parse(const std::span<const char*>& args) const;
        
        const opt* find_by_name(std::string_view name) const;
        const opt* find_by_alias(std::string_view alias) const;
    };

    std::ostream& operator<<(std::ostream& s, const opts& v); 

}