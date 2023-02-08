#pragma once

#include <istream>

namespace lf {

    std::istream& reset_fail_on_eof(std::istream& s);
    
    const char* get_errno_message();
    std::error_code make_io_error_code();

}