#include <catch2/catch_test_macros.hpp>

#include "glob/glob.hpp"

#include <sstream>
#include <iostream>
#include <codecvt>

using namespace lf;

#define PP(expr) std::cout << #expr << ": " << (expr) << std::endl

using g = glob<char>;

// void test_match(const g::matcher& matcher, const std::string& str, size_t repetition, size_t expected_offset, bool expected_match) {
//     std::stringstream stream(str);
//     std::streambuf& buf = *stream.rdbuf(); 
    
//     INFO("matching \"" << str << "\", repetition=" << repetition);

//     bool match = matcher.match(buf, repetition); 
//     REQUIRE( match == expected_match );
    
//     auto offset = buf.pubseekoff(0, std::ios_base::cur, std::ios_base::in);
//     REQUIRE( offset == static_cast<std::streambuf::pos_type>(expected_offset) );
// }

// TEST_CASE("glob string_matcher", "[glob]") {
//     g::string_matcher m("abc");
//     test_match(m, "abcde", 0, m.str.size(), true);
//     test_match(m, "deabc", 0, m.str.size(), false);
//     test_match(m, "a", 0, 1, false);
// }

TEST_CASE("wchar_t string", "[glob]") {
    const wchar_t* str = L"произвольно";
    std::wcout << str << std::endl;
    std::wcout << str[0] << std::endl;
}


/*
TEST_CASE("test", "[glob]") {
    
    std::stringstream ss("abc");
    std::streambuf* buf = ss.rdbuf();

    using iob = std::ios_base;

    PP(buf->sbumpc());
    PP(buf->sbumpc());
    PP(buf->sbumpc());
    PP(buf->sbumpc());

    PP(buf->pubseekpos(0, iob::in));

    PP(buf->pubseekoff(0, iob::cur, iob::in));
    PP(buf->sbumpc());
    PP(buf->pubseekoff(0, iob::cur, iob::in));
    PP(buf->sbumpc());
    PP(buf->pubseekoff(0, iob::cur, iob::in));
    PP(buf->sbumpc());
    PP(buf->pubseekoff(0, iob::cur, iob::in));
    PP(buf->sbumpc());
    PP(buf->pubseekoff(0, iob::cur, iob::in));
    PP(buf->sgetc());

    char bb[10];
    PP(buf->pubseekpos(0, iob::in));
    PP(buf->sgetn(bb, 10));
    PP(bb);
    PP(buf->pubseekoff(0, iob::cur, iob::in));

    PP(buf->pubseekpos(100, iob::in));
    PP(buf->pubseekoff(0, iob::cur, iob::in));

}*/