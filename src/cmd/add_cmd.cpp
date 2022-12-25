#include <filesystem>
#include <iostream>

#include "add_cmd.hpp"

namespace fs = std::filesystem;

namespace lf {

    const char* add_cmd::names[] = { "a", "add" };
    const cmd_desc add_cmd::desc = {names, "<file|shallow-dir/*|recursive-dir/**>+", "add specified file(s) to corresponding index file(s)"};

    add_cmd::add_cmd() : cmd(desc) {
    }

    int add_cmd::run(const std::span<const char*>& args) const {
        if (args.empty()) {
            std::cerr << desc;
            return 1;
        }

        const fs::path cwd = fs::current_path();
        for (const char* path_str: args) {
            fs::path path(path_str);
            if (path.is_relative()) {
                path = cwd / path;
            }
            std::cout << "adding " << path.string() << std::endl;
        }
        
        return 0;
    }
}