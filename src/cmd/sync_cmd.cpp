#include "sync_cmd.hpp"

namespace lf {

const char* names[] = { "s", "sync" };
const char* description = "synchronizes all mirrors";

sync_cmd::sync_cmd(): cmd(cmd_desc {names, description}) {
}

int sync_cmd::run([[maybe_unused]] const std::span<const char*>& args) const {
    fmt::print("Synchronizing!!!\n");
    return 0;
}


}