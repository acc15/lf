#pragma once

#include "tree/tree.hpp"

namespace lf {

    template <tree_concept Tree>
    struct tree_view {
        
        using tree_type = Tree;
        using data_type = typename Tree::data_type;

        /**
         * Sets specified data in current node. 
         * If \p value is default value then node (and also parent nodes maybe removed from tree)
         * @return whether something was changed (node removed or root data changed)
         */
        bool set(const data_type& value);
        
        /**
         * Removes current entry from tree or sets default data if this node is root
         * @return whether something was changed (node removed or root data changed)
         */
        bool remove();

    };

}