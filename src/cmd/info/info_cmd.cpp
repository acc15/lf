#include "cmd/info/info_cmd.hpp"

#include <iostream>
#include <filesystem>

#include "config/config.hpp"

#include "state/state_tree.hpp"
#include "index/index_tree.hpp"

namespace fs = std::filesystem;

namespace lf {

    info_cmd::info_cmd(): cmd(
        {"i", "info"}, 
        "((i|index)|(s|state))", 
        "prints info about indexed files, their sync modes and current synchronization state"
    ) {
    }
    
    int info_cmd::run(const std::span<const char*>& args) const {

        const config cfg = config::load();

        bool print_index = true;
        bool print_state = true;

        if (args.size() > 0) {
            std::string_view arg = args[0];
            if (arg == "i" || arg == "index") {
                print_state = false;
            } else if (arg == "s" || arg == "state") {
                print_index = false;
            } else {
                std::cout << *this;
                return 1;
            }
        }

        const fs::path path = std::filesystem::current_path();
        const config::match_vec v = cfg.find_most_specific_matches(path);
        if (v.empty()) {
            return 1;
        }

        for (const config::match_pair_ptr p: v) {
            if (print_state) {
                print_tree<state_tree>(p->first, p->second.state);
            }
            if (print_index) {
                print_tree<index_tree>(p->first, p->second.index);
            }
        }
        return 0;
    }

}