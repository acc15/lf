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
    REQUIRE_THROWS_AS(load_file(test_dir_path / "no_such_file.txt", t), file_not_found_error);

}