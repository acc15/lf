#include "fs/adv_fstream.hpp"
#include <filesystem>
#include <catch2/catch_test_macros.hpp>

#include "test_util.hpp"

namespace fs = std::filesystem;

using namespace lf;

TEST_CASE("adv_fstream: write read file", "[adv_fstream]") {
    const fs::path p = create_temp_test_dir() / "test.txt";

    adv_ofstream out(p);
    out << "test";
    REQUIRE( out.fd() != 0 );
    out.close();

    std::string str;

    adv_ifstream in(p);
    in >> str;
    REQUIRE( in.fd() != 0 );
    in.close();

    REQUIRE(str == "test");
}

TEST_CASE("adv_fstream: write truncate write read", "[adv_fstream]") {
    const fs::path p = create_temp_test_dir() / "test.txt";

    adv_fstream f(p, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
    REQUIRE( f.is_open() );
    for (size_t i = 0; i < 1024 * 1024; i++) {
        f << '\0';
    }
    f.flush();
    f.truncate();
    REQUIRE( !f.fail() );

    f << "test";

    std::string str;
    f.seekg(0);
    f >> str;
    f.close();

    REQUIRE(str == "test");
}

TEST_CASE("adv_ofstream: shouldnt truncate", "[adv_ofstream]") {
    const fs::path p = create_temp_test_dir() / "test.txt";

    adv_ofstream f1(p);
    f1 << "test";
    f1.close();

    adv_ofstream f2(p, std::ios_base::app);
    f2.close();

    adv_ifstream f3(p);

    std::string str;
    f3 >> str;
    f3.close();

    REQUIRE( str == "test" );
}

TEST_CASE("adv_ofstream: lock unlock", "[adv_fstream]") {

    const fs::path p = create_temp_test_dir() / "test.txt";
    adv_ofstream f1(p);
    REQUIRE( f1.good() );

    f1.lock(true);
    REQUIRE( f1.good() );

    f1.unlock();
    REQUIRE( f1.good() );

}

TEST_CASE("adv_fstream: check initial file and fd", "[adv_fstream]") {

    adv_fstream f;
    REQUIRE( f.file() == nullptr );
    REQUIRE( f.fd() == -1 );

}

TEST_CASE("adv_fstream: truncate must set failbit when no file open", "[adv_fstream]") {

    adv_fstream f;
    REQUIRE( f.good() );

    f.setstate(std::ios_base::eofbit);
    REQUIRE_FALSE( f.good() );

    f.truncate();
    REQUIRE( f.fail() );
    REQUIRE( f.eof() );

}

TEST_CASE("adv_fstream: truncate clears iostate on success", "[adv_fstream]") {

    const fs::path p = create_temp_test_dir() / "test.txt";
    adv_fstream f(p, std::ios_base::in | std::ios_base::out | std::ios_base::app | std::ios_base::ate);

    f.setstate(std::ios_base::eofbit);
    f.truncate();
    REQUIRE(f.good());

}

TEST_CASE("adv_fstream: can write after truncate", "[adv_fstream]") {

    const fs::path p = create_temp_test_dir() / "test.txt";
    adv_ofstream f1(p);
    f1 << "test";
    f1.close();

    adv_fstream f2(p, std::ios_base::in | std::ios_base::out | std::ios_base::app | std::ios_base::ate);
    REQUIRE( f2.good() );
    REQUIRE( f2.tellg() == 4 );

    f2.seekg(0);

    std::string t1;
    f2 >> t1;
    REQUIRE( t1 == "test" );
    REQUIRE( f2.eof() );

    f2.truncate();
    REQUIRE( f2.good() );

    REQUIRE( f2.tellp() == 0 );
    REQUIRE( f2.tellg() == 0 );

    f2 << "overwrite";
    f2.close();

    adv_ifstream f3(p);

    std::string t2;
    f3 >> t2;
    f3.close();

    REQUIRE(t2 == "overwrite");

}

TEST_CASE("adv_fstream: default openmode", "[adv_fstream]") {

    REQUIRE( lf::default_openmode<adv_ifstream>::default_mode == std::ios_base::in );
    REQUIRE( lf::default_openmode<adv_ifstream>::force_mode == std::ios_base::in );
    REQUIRE( lf::default_openmode<adv_ofstream>::default_mode == std::ios_base::out );
    REQUIRE( lf::default_openmode<adv_ofstream>::force_mode == std::ios_base::out );
    REQUIRE( lf::default_openmode<adv_fstream>::default_mode == 0 );
    REQUIRE( lf::default_openmode<adv_fstream>::force_mode == 0 );

}