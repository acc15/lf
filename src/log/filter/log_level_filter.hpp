#pragma once

#include "log/filter/log_filter.hpp"

namespace lf {

    class log_level_filter: public log_filter {
        log_level min_level;

    public:
        explicit log_level_filter(log_level min_level);
        bool level(log_level level) const override;
    };

}