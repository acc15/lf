#include <catch2/catch_test_macros.hpp>

#include "fs/serialization.hpp"
#include "io/format.hpp"
#include "test_util.hpp"

using namespace lf;

struct test_format: format<false> {};

template <typename T>
struct test_serializable {
    static const char* name;
    T data;
};

template <typename T>
const char* test_serializable<T>::name = "test";

template <typename T>
std::istream& operator>>(std::istream& s, with_format<test_format, test_serializable<T>>) {
    return s;
}

template <typename T>
std::ostream& operator<<(std::ostream& s, with_format<test_format, const test_serializable<T>>) {
    return s;
}

TEST_CASE("serialization: write", "[serialization]") {

    std::stringstream s;
    test_serializable<int> t = { 11 };
    s << write_as<test_format>(t);

}

TEST_CASE("serialization: file not found", "[serialization]") {
    test_serializable<int> t = { 12 };

    const std::filesystem::path p = test_dir_path / "no_such_file.txt";
    REQUIRE_FALSE( load_file<test_format>(p, t) );
}
