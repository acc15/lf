#include "util.hpp"

namespace leafsync {

std::chrono::system_clock::time_point ft_to_sys(const std::chrono::file_clock::time_point& v) {
    return std::chrono::file_clock::to_sys(v);
}

}