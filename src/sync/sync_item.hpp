#pragma once

#include <index/sync_mode.hpp>
#include <filesystem>

namespace lf {

    enum class sync_method {
        SYNC,
        FINALIZE,
        POST_CLEANUP
    };

    struct sync_item {
        std::filesystem::path path;
        sync_mode mode;
        sync_method method;
    };

}

