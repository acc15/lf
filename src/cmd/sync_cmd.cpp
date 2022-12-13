#include "sync_cmd.hpp"

namespace leafsync {

const char* sync_cmd::_names[] = { "s", "sync" };

sync_cmd::sync_cmd(): cmd(std::span(_names), "synchronizes all mirrors") {
}

int sync_cmd::run([[maybe_unused]] const std::span<const char*>& args) const {
    return 0;
}


}