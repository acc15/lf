#include "console.hpp"

#include <locale>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace lf {

    void console_init() {
#ifdef WIN32
        SetConsoleOutputCP(CP_UTF8);
#else
        std::locale::global(std::locale("C.UTF-8"));
#endif
    }

}