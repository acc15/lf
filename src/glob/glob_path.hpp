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
            if (m.last) {
                m.cur = m.end;
            } else {
                for (; m.cur != m.end && m.retry > 0; ++m.cur, --m.retry);
            }
            return m.retry == 0;
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

    using element = std::variant<glob::star, std::string, glob>;
    using element_vector = std::vector<element>;

    element_vector elements;

    glob_path(const std::initializer_list<element>& elements_init);

    template<std::ranges::range PathLike>
    bool matches(const PathLike& path) {
        return retryable_match(elements, path, [](auto&& v) {
            return std::visit(glob::star_retryable_visitor{}, v);
        }, [](const glob_path::element& e, match_struct<PathLike>& m) {
            return std::visit(match_visitor {m}, e);
        });
    }

    bool matches(const std::filesystem::path& path);

};

}