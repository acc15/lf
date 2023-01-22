#pragma once

#include "log/sink/log_sink.hpp"

namespace lf {

    class log_stream: public std::ostream {
        log_sink& sink;
        log_message message;
        std::stringbuf buf;
    public:
        log_stream(log_sink& sink, const log_message& msg);
        void send();
    };

}