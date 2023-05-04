#pragma once

#include "cmd/cmd.hpp"
#include "fs/serialization.hpp"
#include "tree/print.hpp"
#include "tree/binary.hpp"

#include <iostream>

namespace lf {
    class info_cmd: public cmd {
    public:
        info_cmd();
        bool run(const opt_map& params) const override;

    private:
        template<serializable_type Tree>
        static void print_tree(const std::filesystem::path& p) {
            try {
                std::cout << load_file<tree_binary_format, Tree>(p);
            } catch (const std::runtime_error& e) {
                log.debug() && log() << e.what() << log::end;
            }
        }
    };
}