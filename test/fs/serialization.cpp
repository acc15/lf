#include <catch2/catch_test_macros.hpp>

#include "fs/serialization.hpp"
#include "io/format.hpp"
#include "test_util.hpp"

using namespace lf;

struct test_serializable {
    static const lf::format format = lf::format::TEXT;
    static const bool binary = false;
    static const char* name;
};

const char* test_serializable::name = "test";

std::istream& operator>>(std::istream& s, with_format<format::TEXT, test_serializable&>) {
    return s;
}

TEST_CASE("file not found", "[serialization]") {
    test_serializable t;

    const std::filesystem::path p = test_dir_path / "no_such_file.txt";

    try {
        load_file(p, t);
        FAIL("no exception was thrown");
    } catch (const file_not_found_error& e) {
        REQUIRE(p == e.path1());
        REQUIRE(e.code().value() == ENOENT);
    }
}
