#pragma once

#include <string>
#include <string_view>

namespace leafsync {
    class error_handler {
    public:

        error_handler(std::string_view source = "", size_t line = 0);

        std::string source;
        size_t line;
        bool has_errors;

        virtual ~error_handler() = default;
        virtual void error(std::string_view msg);

    };
} 
