#pragma once

#include "log/sink/composite_log_sink.hpp"
#include "log/log_stream.hpp"

namespace lf {

    class log: public composite_log_sink {
    public:
        struct end_t {};
        static const end_t end;

        bool trace();
        bool debug();
        bool info();
        bool warn();
        bool error();
        bool with(log_level level);
        static thread_local log_level cur_level;

        log_stream operator()();

    };

    std::ostream& operator<<(std::ostream& s, const log::end_t&);

    extern class log log;

}