#pragma once

#include <filesystem>

namespace leafsync {

std::chrono::system_clock::time_point ft_to_sys(const std::chrono::file_clock::time_point& v);

}