#include <catch2/catch_test_macros.hpp>

#include "glob/glob_path.hpp"

#include <iostream>
#include <ranges>

using namespace lf;

namespace fs = std::filesystem;

TEST_CASE("glob_path: matches", "[glob_path]") {
    
    fs::path p = fs::path("a") / "b" / "c";

    glob_path gp = { "a", glob::star{}, "c.txt" };

    const auto path_str_view = p | std::views::transform([](const fs::path& p) { return p.string(); });
    for (const auto& c: path_str_view) {
        std::cout << c << " / ";
    }

}

template <typename Range>
void make_magic(const Range& r) {

    std::cout << typeid(Range).name() << std::endl;
    std::cout << typeid(std::ranges::const_iterator_t<Range>).name() << std::endl;
    std::cout << typeid(std::ranges::const_sentinel_t<Range>).name() << std::endl;

    std::ranges::const_iterator_t<const Range> iter = r.begin();
    std::ranges::const_sentinel_t<const Range> end = r.end();

}

TEST_CASE("glob_path: types", "[glob_path]") {

    const glob::element_vector v = {"a", "b"};
    std::cout << typeid(glob::element_vector).name() << std::endl;
    std::cout << typeid(std::ranges::const_iterator_t<glob::element_vector>).name() << std::endl;
    std::cout << typeid(std::ranges::const_sentinel_t<glob::element_vector>).name() << std::endl;

    std::ranges::const_iterator_t<const glob::element_vector> i = v.begin();

    make_magic(v);

}