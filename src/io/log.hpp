#pragma once

#include <functional>
#include <ostream>

namespace lf {

    struct log {

        enum level { TRACE, DEBUG, INFO, WARN, ERROR, MUTE };
        static const char* const level_names[6];

        bool trace();
        bool debug();
        bool info();
        bool warn();
        bool error();
        bool with(level level);

        std::ostream& operator()();

        level min_level = get_default_min_level();
        level cur_level = default_level;
        std::ostream* stream = nullptr;

        static const char* const env_name;
        static const level default_level = INFO;
        static level get_default_min_level();

        std::ostream& default_stream(level level);

    };

    extern struct log log;

}