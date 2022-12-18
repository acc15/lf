#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <cstdlib>
#include <string>
#include <fstream>
#include <fmt/core.h>

#include <filesystem>
#include <Windows.h>

//void print_lines(const std::filesystem::path& p) {
//    std::ifstream stream(p);
//    std::string line;
//    while (std::getline(stream, line)) {
//        fmt::print("Line read: {}\n", line);
//    }
//}

TEST_CASE("print unicode", "[unicode]") {
    fmt::print("русский текст\n");
}

TEST_CASE("char8_t size", "[unicode]") {
    fmt::print("char8_t size {}\n", sizeof(char8_t));
}

TEST_CASE("unicode env", "[unicode]") {
    // const wchar_t* wenv = _wgetenv(L"UNICODE_TEST");
    const char* actual_env = getenv("UNICODE_TEST");
    const char* expect_env = "русский текст";
    fmt::print("actual={} expected={}\n", actual_env, expect_env);
    REQUIRE_THAT(actual_env, Catch::Matchers::Equals(expect_env));
}

TEST_CASE("load", "[config]") {
    std::filesystem::path root(getenv("CMAKE_SOURCE_DIR"));
    std::filesystem::path unicode_dir = root / "test" / "unicode";
    for (const auto e : std::filesystem::directory_iterator(unicode_dir)) {
        const auto p = e.path();
        fmt::print("entry: {}\n", p.string());
        //print_lines(e.path());
    }
}