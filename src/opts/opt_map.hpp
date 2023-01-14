#pragma once

#include <string_view>
#include <vector>
#include <unordered_map>

namespace lf {

    class opt_map {
    public:
        using vec_type = std::vector<std::string_view>; 
        using map_type = std::unordered_map<std::string_view, vec_type>;

        opt_map(map_type&& map);

        bool has(std::string_view name) const;
        const vec_type& operator[](std::string_view name) const;

    private:
        static const vec_type empty_vec;
        map_type _map;
    };

}