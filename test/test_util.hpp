#pragma once

#include <vector>
#include <filesystem>
#include <source_location>

#include <io/errors.hpp>

namespace lf {

    extern const std::filesystem::path test_dir;
    extern const std::filesystem::path test_root;

    std::filesystem::path test_path(std::string_view p); 
    std::string test_pstr(std::string_view p); 

    class test_errors: public errors {
    protected:
        void on_error(std::string_view msg) override;

    public:
        test_errors(const data_location& loc);
        std::vector<std::string> messages;
    };

}