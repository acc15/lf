#include <functional>
#include <ostream>

namespace lf {

    class log {
    public:

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

        std::function<std::ostream&(enum level)> stream_fn = std::bind(&log::default_stream_fn, this, std::placeholders::_1);

        static const level default_level = INFO;
        static level get_default_min_level();
        static const char* level_names[];

    private:
        std::ostream& default_stream_fn(level level);

    };

    extern class log log;

}