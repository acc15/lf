#include "log/formatter/simple_log_formatter.hpp"

#include "io/time.hpp"

#include <sstream>

namespace lf {

    std::string simple_log_formatter::format(const log_message& msg) const {
        std::ostringstream s;
        s   << "[" << format_date_time(msg.timestamp) << "]"
            << "[" << std::setw(5) << std::setfill(' ') << msg.level << "] " 
            << msg.text 
            << std::endl;
        return s.str();
    }

}