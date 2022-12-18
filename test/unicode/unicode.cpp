#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <string>
#include <fstream>
#include <locale>

#include <fmt/core.h>
#include <filesystem>

void print_lines(const std::filesystem::path& p) {
    std::string line;
    std::ifstream stream(p);
    while (std::getline(stream, line)) {
        fmt::print("Line read: {}\n", line);
    }
}

TEST_CASE("print unicode", "[unicode]") {
    fmt::print("русский текст\n");
}

TEST_CASE("char8_t size", "[unicode]") {
    fmt::print("char8_t size {}\n", sizeof(char8_t));
}

TEST_CASE("unicode env", "[unicode]") {
    // const wchar_t* wenv = _wgetenv(L"UNICODE_TEST");
    std::string actual_env = getenv("UNICODE_TEST");
    std::string expect_env = "русский текст";
    fmt::print("actual={} expected={}\n", actual_env, expect_env);
    REQUIRE( actual_env == expect_env );
}

TEST_CASE("list_dir", "[config]") {
    const auto cur = std::filesystem::current_path();
    fmt::print("current_path={}\n", cur.string());
    std::filesystem::path unicode_dir = cur / "test" / "unicode" / "testdir";
    for (const auto& e : std::filesystem::directory_iterator(unicode_dir)) {
        const auto p = e.path();
        fmt::print("entry: {}\n", p.string());
        print_lines(e.path());
    }
}

TEST_CASE("list_utf8_file", "[config]") {
    print_lines(std::filesystem::current_path() / "test" / "unicode" / "testdir" / "Русский.txt");
}