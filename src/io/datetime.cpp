#include "io/datetime.hpp"
#include "io/format_stream.hpp"

#include <exception>
#include <iomanip>
#include <time.h>

namespace lf {

    std::ostream& operator<<(std::ostream& s, const format_date_time& fmt) {
        std::time_t t = std::chrono::system_clock::to_time_t(fmt.tp);
        std::tm tm;
        if ((fmt.utc ? gmtime_r(&t, &tm) : localtime_r(&t, &tm)) == nullptr) {
            throw std::runtime_error(format_stream() << "unable to convert time_t to tm: " << t);
        }
        return s << std::put_time(&tm, fmt.format.c_str());
    }

}