#include <iostream>

#include "cmd/cmd_registry.hpp"
#include "util/console.hpp"
#include "log/log_config.hpp"
#include "log/log.hpp"

using namespace lf;

int main(const int argc, const char *argv[]) {
    console_init();
    log_init();
    std::span<const char*> args(argv, argc);
    try {
        return cmds.run(args.subspan(1));
    } catch (const std::runtime_error& e) {
        log.error() && log() << e.what() << log::end;
        return 1;
    } catch (...) {
        log.error() && log() << "unknown error occured" << log::end;
        return 1;
    }
}
