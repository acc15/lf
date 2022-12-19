#pragma once

#include <vector>
#include <filesystem>
#include <source_location>

#include <err/error_handler.hpp>

namespace leafsync {

    extern const std::filesystem::path test_dir;

    class test_error_handler: public error_handler {
    public:
        void error(std::string_view msg) override;

        std::vector<std::string> messages;
    };

}