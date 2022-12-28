#include "fs/util.hpp"

namespace lf {

    std::chrono::system_clock::time_point ft_to_sys(const std::chrono::file_clock::time_point& v) {
        // TODO: std::chrono::clock_cast not implemented in GNU C++ library (however it provides file_clock::to_sys)
        // After implementation of clock_cast in GNU lib - this #ifdef should be replaced with clock_cast
#ifdef __GNUG__
        return std::chrono::file_clock::to_sys(v);
#else
        return std::chrono::clock_cast<std::chrono::system_clock>(v);
#endif
    }

}