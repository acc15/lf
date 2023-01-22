#pragma once

#include "log/log_message.hpp"

namespace lf {

    class log_filter {
    public:
        virtual ~log_filter() = default;
        virtual bool level(log_level level) const = 0;
        virtual bool filter(const log_message& msg) const;
    };

}