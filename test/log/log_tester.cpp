#include <algorithm>

#include "log/log_tester.hpp"

#include "log/log.hpp"
#include "log/filter/log_level_filter.hpp"

namespace lf {

    log_vector_sink::log_vector_sink(std::vector<log_message>& messages): messages(messages) {
    }

    void log_vector_sink::write(const log_message& message) {
        messages.push_back(message);
    }

    log_tester::log_tester(log_level level) {
        log.filter = std::make_shared<log_level_filter>(level);
        log.sinks = { std::make_shared<log_vector_sink>(messages) };
    }

    log_tester::~log_tester() {
        log.filter.reset();
        log.sinks.clear();
    }

    bool log_tester::contains(std::string_view v) const {
        return std::any_of(messages.begin(), messages.end(), [v](const log_message& msg) { 
            return msg.text.find(v) != std::string::npos; 
        });
    }

}