#pragma once

#include "io/log/log_message.hpp"

namespace lf {

    class log_formatter {
    public:
        virtual ~log_formatter() = default;
        virtual std::string format(const log_message& msg) const;
    };

}