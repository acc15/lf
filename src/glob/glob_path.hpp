#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "glob/glob.hpp"
#include "glob/match.hpp"

namespace lf {

class glob_path {

    template <std::ranges::range Sequence>
    struct match_visitor {
        match_struct<Sequence>& m;

        bool operator()(const glob::star&) {
            ++m.cur;
            return true;
        }

        bool operator()(const std::string& s) {
            if (*m.cur == s) {
                ++m.cur;
                return true;
            }
            return false;
        }

        bool operator()(const glob& g) {
            if (g.matches(*m.cur)) {
                ++m.cur;
                return true;
            }
            return false;
        }
        
    };

public:

    using element = std::variant<glob::star, std::string, glob>;
    using element_vector = std::vector<element>;

    element_vector elements;

    glob_path(const std::initializer_list<element>& elements_init);

    template<std::ranges::range PathLike>
    bool matches(const PathLike& path) {
        return glob_match(elements, path, [](const glob_path::element& e, match_struct<PathLike>& m) {
            return std::visit(match_visitor<PathLike> {m}, e);
        });
    }

    bool matches(const std::filesystem::path& path);

};

}