#pragma once

#include "log/sink/log_sink.hpp"

#include <ostream>

namespace lf {

    class stream_log_sink: public log_sink {
        std::ostream& stream;
    protected:
        void write(const log_message& messsage) override;
    public:
        explicit stream_log_sink(std::ostream& stream);
    };

}