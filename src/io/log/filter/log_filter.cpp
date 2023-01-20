#include "io/log/filter/log_filter.hpp"

namespace lf {

    bool log_filter::filter(const log_message& msg) const {
        return level(msg.level);
    }

}