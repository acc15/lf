#include "cmd/cmd_registry.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

int main(const int argc, const char *argv[]) {

#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif
    
    std::span<const char*> args(argv, argc);
    return leafsync::cmds.run(args.subspan(1));

}
