#include "log/sink/log_sink.hpp"

namespace lf {

    void log_sink::send(const log_message& message) {
        if (filter && !filter->filter(message)) {
            return;
        }
        if (formatter) {
            write(log_message { .level = message.level, .timestamp = message.timestamp, .text = formatter->format(message) });
        } else {
            write(message);
        }
    }

}