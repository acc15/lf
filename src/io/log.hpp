#include <functional>
#include <ostream>

namespace lf {

class log {
public:
    enum level {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        MUTE
    };

    bool trace();
    bool debug();
    bool info();
    bool warn();
    bool error();
    bool with(enum level level);

    std::ostream& operator()();

    enum level min_level = INFO;
    enum level default_level = INFO;
    enum level next_level = default_level;

    std::function<std::ostream&(enum level)> stream_fn = std::bind(&log::default_stream_fn, this, std::placeholders::_1);

private:
    std::ostream& default_stream_fn(enum level level);

};

extern class log log;

}