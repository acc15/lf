#include <catch2/catch_test_macros.hpp>

#include "glob/glob.hpp"
#include <encoding/utf8.hpp>

#include <sstream>

using namespace lf;

#define PP(expr) std::cout << #expr << ": " << (expr) << std::endl

using g = glob<utf8_encoding>;

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