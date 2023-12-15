#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "glob/glob.hpp"
#include "glob/match.hpp"

namespace lf {

class glob_path {

public:

    using element = std::variant<glob::star, std::string, glob>;
    using element_vector = std::vector<element>;

    element_vector elements;

    glob_path(const std::initializer_list<element>& elements_init);

    template<std::ranges::range PathLike>
    bool matches(const PathLike& path) {
        return retryable_match(elements, path, [](auto&& v) {
            return std::visit(glob::star_retryable_visitor{}, v);
        }, [](const glob_path::element& e, auto& begin, const auto& end, size_t repetition, bool last) {
            return false; // std::visit(match_visitor {begin, end, repetition, last}, e);
        });
    }

    bool matches(const std::filesystem::path& path);

};

}