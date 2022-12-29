#pragma once

#include "cmd/cmd.hpp"
#include "io/serialization.hpp"
#include "tree/tree_print.hpp"
#include "tree/tree_binary.hpp"

#include <iostream>

namespace lf {
    class info_cmd: public cmd {
    public:
        info_cmd();
        int run(const std::span<const char*>& args) const override;

    private:
        template<serializable Tree>
        static void print_tree(const std::string& sync, const std::filesystem::path& p) {
            Tree t;
            if (load_file(p, t)) {
                std::cout << Tree::name  << " of \"" << sync << "\" sync: " << std::endl << t;
            }
        }
    };
}