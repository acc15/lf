#pragma once

#include <vector>
#include <string>
#include <sstream>

#include "log/sink/log_sink.hpp"
#include "log/log_level.hpp"

namespace lf {

    class log_vector_sink: public log_sink {
        std::vector<log_message>& messages;
    public:
        log_vector_sink(std::vector<log_message>& messages);
        virtual void write(const log_message& message);
    };

    class log_tester {
    public:
        log_tester(log_level level = log_level::INFO);
        ~log_tester();
        
        bool contains(std::string_view v) const;

        std::vector<log_message> messages;
    };

}