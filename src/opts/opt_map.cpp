#include "opts/opt_map.hpp"

namespace lf {

    const opt_map::vec_type opt_map::empty_vec = {};

    opt_map::opt_map(map_type&& map): _map(map) {
    }

    bool opt_map::has(std::string_view name) const {
        return _map.contains(name);
    }

    const opt_map::vec_type& opt_map::operator[](std::string_view name) const {
        auto it = _map.find(name);
        return it == _map.end() ? empty_vec : it->second;
    }

}