#include "cmd/cmd_registry.hpp"
#include "util/console.hpp"

int main(const int argc, const char *argv[]) {
    lf::console_init();    

    std::span<const char*> args(argv, argc);
    return lf::cmds.run(args.subspan(1));
}
