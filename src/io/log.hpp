#include <functional>
#include <ostream>

namespace lf {

    struct log {

        enum level { TRACE, DEBUG, INFO, WARN, ERROR, MUTE };

        bool trace();
        bool debug();
        bool info();
        bool warn();
        bool error();
        bool with(level level);

        std::ostream& operator()();

        level min_level = get_default_min_level();
        level reset_level = default_level;
        level next_level = reset_level;
        std::ostream* stream = nullptr;

        static const level default_level = INFO;
        static const char* level_names[];
        static level get_default_min_level();

        std::ostream& default_stream(level level);

    };

    extern struct log log;

}