#include "log/log.hpp"

namespace lf {

    thread_local log_level log::cur_level = log::default_level;

    const log::end_t log::end;

    bool log::trace() { return with(TRACE); }
    bool log::debug() { return with(DEBUG); }
    bool log::info() { return with(INFO); }
    bool log::warn() { return with(WARN); }
    bool log::error() { return with(ERROR); }
    
    bool log::with(log_level level) {
        if (filter && !filter->level(level)) {
            return false;
        }
        cur_level = level;
        return true;
    }

    log_stream log::operator()() {
        log_message msg = { .level = cur_level, .timestamp = std::chrono::system_clock::now() };
        cur_level = default_level;
        return log_stream(*this, msg);
    }

    std::ostream& operator<<(std::ostream& s, const log::end_t&) {
        log_stream* lp = dynamic_cast<log_stream*>(&s);
        if (lp != nullptr) {
            lp->send();
        }
        return s;
    }

    class log log;

}