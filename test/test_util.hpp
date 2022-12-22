#pragma once

#include <vector>
#include <filesystem>
#include <source_location>

#include <err/error_handler.hpp>

namespace leafsync {

    extern const std::filesystem::path test_dir;
    extern const std::filesystem::path test_root;

    std::filesystem::path test_path(std::string_view p); 
    std::string test_pstr(std::string_view p); 

    class test_error_handler: public error_handler {
    public:
        void error(std::string_view msg) override;

        std::vector<std::string> messages;
    };

}