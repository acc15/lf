#include "io/time.hpp"
#include "io/format_stream.hpp"
#include "fs/time.hpp"
#include "util/string.hpp"

#include <exception>
#include <iomanip>

#include <time.h>

namespace lf {

    const std::string_view ms_format_letter = "%L";

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
        using namespace std::chrono;
        using str = std::string;
        
        str format = static_cast<str>(fmt.format);

        str::size_type ms_index = format.find(ms_format_letter);
        if (ms_index != str::npos) {
            
            system_clock::duration d = fmt.tp.time_since_epoch();
            d -= duration_cast<seconds>(d);

            milliseconds::rep ms = duration_cast<milliseconds>(d).count();
            std::string ms_str = lpad(std::to_string(ms), 3, '0');
            do {
                format.replace(ms_index, ms_format_letter.size(), ms_str);
                ms_index += ms_str.size();
                ms_index = format.find(ms_format_letter, ms_index);
            } while (ms_index != str::npos);
        }
        
        std::time_t t = system_clock::to_time_t(fmt.tp);
        std::tm tm;
        time_to_tm(fmt.utc, t, tm);
        return s << std::put_time(&tm, format.c_str());
    }

    std::ostream& operator<<(std::ostream& s, const format_date_time_s<std::chrono::file_clock::time_point>& fmt) {
        return s << format_date_time_s<std::chrono::system_clock::time_point> { file_clock_to_system(fmt.tp), fmt.utc, fmt.format };
    }

}