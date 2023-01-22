#include <iostream>

#include "cmd/cmd_registry.hpp"
#include "util/console.hpp"
#include "log/config.hpp"

int main(const int argc, const char *argv[]) {
    lf::console_init();
    lf::log_init();
    std::span<const char*> args(argv, argc);
    try {
        return lf::cmds.run(args.subspan(1));
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "unknown error occured" << std::endl;
        return 1;
    }
}
