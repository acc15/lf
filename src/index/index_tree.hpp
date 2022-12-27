#pragma once

#include "../tree/tree.hpp"
#include "sync_mode.hpp"

namespace lf {
    using index_tree = tree<sync_mode>;
}