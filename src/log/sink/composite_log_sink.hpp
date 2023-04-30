#pragma once

#include "log/sink/log_sink.hpp"

#include <memory>
#include <vector>

namespace lf {

    class composite_log_sink: public log_sink {
    protected:
        void write(const log_message& message) override;

    public:
        std::vector<std::shared_ptr<log_sink>> sinks;
    };

}