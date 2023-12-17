#pragma once

#include <vector>
#include <string>
#include <filesystem>

#include "glob/match.hpp"
#include "glob/glob.hpp"

namespace lf {

class glob_path {

    template <std::ranges::range Sequence>
    struct match_visitor {
        match_struct<Sequence>& m;

        bool operator()(const globstar&) {
            if (m.cur == m.end) {
                return false;
            }
            ++m.cur;
            return true;
        }

        bool operator()(const std::string& s) {
            if (m.cur == m.end || *m.cur != s) {
                return false;
            }
            ++m.cur;
            return true;
        }

        bool operator()(const glob& g) {
            if (m.cur == m.end || !g.matches(*m.cur)) {
                return false;
            }
            ++m.cur;
            return true;
        }
        
    };

public:

    using element = std::variant<globstar, std::string, glob>;
    using element_vector = std::vector<element>;

    element_vector elements;

    glob_path(const std::initializer_list<element>& elements_init);

    template<std::ranges::range PathLike>
    bool matches(const PathLike& path) {
        return glob_match(elements, path, [](const glob_path::element& e, match_struct<PathLike>& m) {
            return std::visit(match_visitor {m}, e);
        });
    }

    bool matches(const std::filesystem::path& path);

};

}