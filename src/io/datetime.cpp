#include "io/datetime.hpp"
#include "io/format_stream.hpp"

#include <exception>
#include <iomanip>

#include <time.h>

namespace lf {

    std::chrono::system_clock::time_point file_clock_to_system(const std::chrono::file_clock::time_point& v) {
        // TODO: std::chrono::clock_cast not implemented in GNU C++ library (however it provides file_clock::to_sys)
        // After implementation of clock_cast in GNU lib - this #ifdef should be replaced with clock_cast
#ifdef __GNUG__
        return std::chrono::file_clock::to_sys(v);
#else
        return std::chrono::clock_cast<std::chrono::system_clock>(v);
#endif
    }

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

    std::ostream& operator<<(std::ostream& s, const format_date_time_s<std::chrono::system_clock::time_point>& fmt) {
        std::time_t t = std::chrono::system_clock::to_time_t(fmt.tp);
        std::tm tm;
        time_to_tm(fmt.utc, t, tm);
        return s << std::put_time(&tm, fmt.format.c_str());
    }

    std::ostream& operator<<(std::ostream& s, const format_date_time_s<std::chrono::file_clock::time_point>& fmt) {
        return s << format_date_time_s<std::chrono::system_clock::time_point> { file_clock_to_system(fmt.tp), fmt.utc, fmt.format };
    }

}