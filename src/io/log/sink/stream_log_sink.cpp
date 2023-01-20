#include "io/log/sink/stream_log_sink.hpp"

namespace lf {
    
    void stream_log_sink::write(const log_message& message) {
        stream << message.text;
    }

    stream_log_sink::stream_log_sink(std::ostream& stream): stream(stream) {
    }

}