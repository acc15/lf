#include "io/log/sink/composite_log_sink.hpp"

namespace lf {

    void composite_log_sink::write(const log_message& message) {
        for (auto& sink: sinks) {
            sink->send(message);
        }
    }

}