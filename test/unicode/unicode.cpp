﻿#include <catch2/catch_test_macros.hpp>

#include <cstring>
#include <string>
#include <fstream>
#include <filesystem>
#include <locale>
#include <set>

#include "test_util.hpp"

using namespace lf;

const std::filesystem::path unicode_dir = test_dir_path / "unicode" / "testdir";

TEST_CASE("unicode env", "[unicode]") {   
    const char* expect_env = "русский текст";
#ifdef _WIN32
    _putenv_s("UNICODE_TEST", expect_env);
#else
    setenv("UNICODE_TEST", expect_env, 1);
#endif
    const char* actual_env = getenv("UNICODE_TEST");
    REQUIRE( std::strcmp(actual_env, expect_env) == 0 );
}

TEST_CASE("unicode list dir", "[unicode]") {
    std::set<std::string> actual_names;
    for (const auto& e : std::filesystem::directory_iterator(unicode_dir)) {
        actual_names.insert(e.path().filename().string());
    }

    std::set<std::string> expect_names = {"Ελληνικά.txt", "Русский.txt"};
    REQUIRE( actual_names == expect_names );
}

TEST_CASE("unicode list utf8 file", "[unicode]") {
    std::vector<std::string> expect_lines = { "Русский текст" };
    std::vector<std::string> actual_lines;

    std::string line;
    std::ifstream stream(unicode_dir / "Русский.txt");
    while (std::getline(stream, line)) {
        actual_lines.push_back(line);
    }

    REQUIRE( actual_lines == expect_lines );
}
