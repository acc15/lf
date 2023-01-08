#include "io/datetime.hpp"
#include "io/format_stream.hpp"

#include <exception>
#include <iomanip>

#include <time.h>

namespace lf {

    void time_to_tm(bool utc, const std::time_t& t, std::tm& tm) {
#ifdef _WIN32
        errno_t err = utc ? gmtime_s(&tm, &t) : localtime_s(&tm, &t);
        if (err != 0) {
            throw std::system_error(err, std::generic_category(), format_stream() << "unable to convert time_t " << t << " to tm");
        }
#else
        std::tm* ptr = utc ? gmtime_r(&t, &tm) : localtime_r(&t, &tm);
        if (ptr != &tm) {
            throw std::system_error(errno, std::generic_category(), format_stream() << "unable to convert time_t " << t << " to tm");
        }
#endif
    }

    std::ostream& operator<<(std::ostream& s, const format_date_time& fmt) {
        std::time_t t = std::chrono::system_clock::to_time_t(fmt.tp);
        std::tm tm;

#ifdef _WIN32
        errno_t err = fmt.utc ? gmtime_s(&tm, &t) : localtime_s(&tm, &t);
        if (err != 0) {
            throw std::system_error(err, std::generic_category(), format_stream() << "unable to convert time_t " << t << " to tm");
        }
#else
        std::tm* ptr = fmt.utc ? gmtime_r(&t, &tm) : localtime_r(&t, &tm);
        if (ptr != &tm) {
            throw std::system_error(errno, std::generic_category(), format_stream() << "unable to convert time_t " << t << " to tm");
        }
#endif

        return s << std::put_time(&tm, fmt.format.c_str());
    }

}