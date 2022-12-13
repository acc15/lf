#include <iostream>
#include <span>
#include <filesystem>

// #define FMT_USE_NONTYPE_TEMPLATE_ARGS true
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/compile.h>

#include "fs/util.hpp"
#include "cmd/cmd_registry.hpp"

namespace fs = std::filesystem;
using namespace fmt::literals;
using namespace leafsync;



int main(const int argc, const char *argv[]) {

    std::cout << leafsync::cmds.usage() << std::endl;

    const std::span<const char*> args(argv, argc);
    for (const auto& a: args) {
        fmt::print(FMT_COMPILE("{}\n"), a);
    }

    fs::path p("/mnt/router");
    for (const fs::directory_entry& e: fs::directory_iterator(p)) {
        fmt::print(FMT_COMPILE("{} isdir:{} mod:{:%FT%T} perms:{:o}\n"), 
            e.path().string(), 
            e.is_directory(), 
            leafsync::ft_to_sys(e.last_write_time()),
            static_cast<unsigned>(e.status().permissions()));

    }
    return 0;
}
