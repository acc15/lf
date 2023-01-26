#pragma once

#include <index/sync_mode.hpp>
#include <filesystem>

namespace lf {

    struct sync_item {
        std::filesystem::path path;
        sync_mode mode;
        bool cleanup;
    };

}

