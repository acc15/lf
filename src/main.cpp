
#include "cmd/cmd_registry.hpp"

int main(const int argc, const char *argv[]) {
    std::span<const char*> args(argv, argc);
    return leafsync::cmds.run(args.subspan(1));
}
