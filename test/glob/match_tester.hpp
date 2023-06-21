#pragma once

#include <glob/glob.hpp>
#include <encoding/utf8.hpp>
#include <sstream>

namespace lf {

    using test_glob = glob<utf8_encoding>;

    class match_tester {
    public:
        using stringstream = std::basic_stringstream<test_glob::char_type, test_glob::char_traits>;
        stringstream stream;
        test_glob::streambuf* buf;

        match_tester(const test_glob::string& string);
        std::size_t pos() const;
        std::size_t size() const;
    };


}