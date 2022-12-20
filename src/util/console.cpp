#include "console.hpp"

#include <locale>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace leafsync {

    void console_init() {
        std::locale::global(std::locale("C.UTF-8"));
#ifdef WIN32
        SetConsoleOutputCP(CP_UTF8);
#endif
    }

}