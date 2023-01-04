
#pragma once

#include "tree/tree.hpp"

namespace lf {

    template <tree_concept Tree, typename NameCompare = std::less<std::string>>
    struct tree_entry_name_order {
        using entry_ptr = typename Tree::entry_ptr;
        const NameCompare less = {};
        bool operator()(entry_ptr l, entry_ptr r) {
            return less(l->first, r->first);
        }
    };

}

