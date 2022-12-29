#include <iostream>
#include "sync_cmd.hpp"

namespace lf {

    sync_cmd::sync_cmd(): cmd({ "s", "sync" },  "[list of sync names, all if empty]+", "synchronizes mirrors") {
    }

    int sync_cmd::run([[maybe_unused]] const std::span<const char*>& args) const {
        std::cout << "Synchronizing!!!" << std::endl;
        return 0;
    }

}