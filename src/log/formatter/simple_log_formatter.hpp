#pragma once

#include "log/formatter/log_formatter.hpp"

namespace lf {

    class simple_log_formatter: public log_formatter {
    public:
        std::string format(const log_message& msg) const override;
    };

}