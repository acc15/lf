#include "log/filter/log_level_filter.hpp"

namespace lf {

    log_level_filter::log_level_filter(log_level min_level): min_level(min_level) {
    }

    bool log_level_filter::level(log_level level) const {
        return level >= min_level;
    }

}
