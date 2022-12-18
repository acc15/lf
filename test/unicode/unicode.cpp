#include <catch2/catch_test_macros.hpp>

#include <cstdlib>
#include <string>
#include <fstream>
#include <fmt/core.h>

#include <filesystem>
// #include <Windows.h>

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
    const char* actual_env = getenv("UNICODE_TEST");
    const char* expect_env = "русский текст";
    fmt::print("actual={} expected={}\n", actual_env, expect_env);
    REQUIRE( std::strcmp(actual_env, expect_env) == 0 );
}

TEST_CASE("list_dir", "[config]") {
    fmt::print("current_path={}\n", std::filesystem::current_path().string());
    std::filesystem::path unicode_dir = std::filesystem::current_path() / "test" / "unicode" / "testdir";
    for (const auto& e : std::filesystem::directory_iterator(unicode_dir)) {
        const auto p = e.path();
        fmt::print("entry: {}\n", p.string());
        print_lines(e.path());
    }
}