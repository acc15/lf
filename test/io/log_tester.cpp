#include "io/log_tester.hpp"

#include "log/log.hpp"
#include "log/filter/log_level_filter.hpp"
#include "log/sink/stream_log_sink.hpp"

namespace lf {

    log_tester::log_tester(log_level level) {
        log.filter = std::make_unique<log_level_filter>(level);
        log.sinks.clear();
        log.sinks.push_back(std::make_unique<stream_log_sink>(out));
    }

    log_tester::~log_tester() {
        log.filter.reset();
        log.sinks.clear();
    }

    std::string log_tester::str() const {
        return out.str();
    }

}