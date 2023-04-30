#include "log/log_stream.hpp"

#include "io/null_stream.hpp"

namespace lf {

    log_stream::log_stream(log_sink& sink, const log_message& msg): 
        std::ostream(sink.filter && !sink.filter->level(msg.level) 
            ? static_cast<std::streambuf*>(&nullout.nullbuf) 
            : &buf), 
        sink(sink), 
        message(msg) 
    {
    }

    void log_stream::send() {
        if (buf.view().empty()) {
            return;
        }
        message.text = buf.str();
        sink.send(message);
        buf.str("");
    }

}