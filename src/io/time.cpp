#include "io/time.hpp"
#include "io/format_stream.hpp"
#include "fs/time.hpp"
#include "util/string.hpp"

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

    bool next_nano_spec_char_match(std::string::size_type match_len, char ch) {
        switch (match_len) {
            case 0: return ch == '%';
            case 1: return ch == 'N';
            default: return std::isdigit(ch);
        }
    }

    std::string::size_type next_nano_spec(const std::string& str, std::string::size_type pos) {
        using sz = std::string::size_type;
        
        sz match_len = 0;
        for (; pos < str.size(); ++pos) {
            char ch = str[pos];
            if (!next_nano_spec_char_match(match_len, ch)) {
                match_len = 0;
                continue;
            }

            ++match_len;
            if (match_len == 4) {
                return pos - (match_len - 1);
            }
        }
        return std::string::npos;
    } 

    std::ostream& operator<<(std::ostream& s, const format_date_time_s<std::chrono::system_clock::time_point>& fmt) {
        using namespace std::chrono;
        using str = std::string;
        
        str format = static_cast<str>(fmt.format);

        // %N03 - millis, %N33 - microseconds, %N63 - nanos
        // %N09 - whole nanos (9 digits)

        str::size_type spec_index = next_nano_spec(format, 0);
        if (spec_index != str::npos) { 
            system_clock::duration d = fmt.tp.time_since_epoch();
            nanoseconds::rep ns = duration_cast<nanoseconds>(d - duration_cast<seconds>(d)).count();
            std::string ns_str = lpad(std::to_string(ns), 9, '0');
            do {
                std::string::size_type start = format[spec_index + 2] - '0';
                std::string::size_type count = format[spec_index + 3] - '0';
                format.replace(spec_index, 4, ns_str.substr(start, count));
                spec_index = next_nano_spec(format, spec_index + count);
            } while (spec_index != str::npos);
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