#include "fs/adv_fstream.hpp"
#include <filesystem>
#include <catch2/catch_test_macros.hpp>

#include "test_util.hpp"

namespace fs = std::filesystem;

using namespace lf;

TEST_CASE("adv_fstream write read file", "[adv_fstream]") {
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

TEST_CASE("adv_fstream write truncate write read", "[adv_fstream]") {
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

TEST_CASE("adv_ofstream shouldnt truncate", "[adv_ofstream]") {
    const fs::path p = create_temp_test_dir() / "test.txt";

    adv_ofstream f1(p);
    f1 << "test";
    f1.close();

    adv_ofstream f2(p);
    f2.close();

    adv_ifstream f3(p);

    std::string str;
    f3 >> str;
    f3.close();

    REQUIRE( str == "test" );
}

TEST_CASE("adv_ofstream lock unlock", "[adv_fstream]") {

    const fs::path p = create_temp_test_dir() / "test.txt";
    adv_ofstream f1(p);
    REQUIRE( f1.lock(LOCK_EXCLUSIVE) );
    REQUIRE( f1.unlock() );

}
