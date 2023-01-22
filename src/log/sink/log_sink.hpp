#pragma once

#include "log/log_message.hpp"
#include "log/formatter/log_formatter.hpp"
#include "log/filter/log_filter.hpp"

#include <memory>

namespace lf {

    class log_sink {
    public:
        std::unique_ptr<log_formatter> formatter;
        std::unique_ptr<log_filter> filter;

        virtual ~log_sink() = default;
        void send(const log_message& message); 

    protected:
        virtual void write(const log_message& message) = 0;
    };

}