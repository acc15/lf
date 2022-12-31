#pragma once

#include "cmd/cmd.hpp"
#include "fs/serialization.hpp"
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
            try {
                const Tree t = load_file<Tree>(p);
                std::cout << Tree::name << " of \"" << sync << "\" sync: " << std::endl << t;
            } catch (const std::runtime_error& e) {
                log.debug() && log() << e.what() << std::endl;
            }
        }
    };
}