#include "cmd/cmd_registry.hpp"
#include "util/console.hpp"

int main(const int argc, const char *argv[]) {
    leafsync::console_init();    

    std::span<const char*> args(argv, argc);
    return leafsync::cmds.run(args.subspan(1));
}
