#include <iostream>
#include "cmd_desc.hpp"
#include "sync_cmd.hpp"

namespace lf {

const char* sync_cmd::names[] = { "s", "sync" };
const cmd_desc sync_cmd::desc = { 
    names, 
    "[list of sync names, all if empty]+", 
    "synchronizes mirrors"
};

sync_cmd::sync_cmd(): cmd(desc) {
}

int sync_cmd::run([[maybe_unused]] const std::span<const char*>& args) const {
    std::cout << "Synchronizing!!!" << std::endl;
    return 0;
}


}