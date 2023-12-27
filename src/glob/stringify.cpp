#include "glob/stringify.hpp"

namespace lf {

struct stringify_visitor {
    std::string& result;

    void operator()(const glob::star&) {
        result.append(1, '*');
    }

    void operator()(const glob::any&) {
        result.append(1, '?');
    }

    void operator()(const glob::range& range) {
        result.append(1, '[');
        if (range.inverse) {
            result.append(1, '!');
        }
        for (const auto& e: range.map) {
            if (e.first == e.second) {
                utf8::unchecked::append(e.first, std::back_inserter(result));
            } else {
                utf8::unchecked::append(e.first, std::back_inserter(result));
                result.append(1, '-');
                utf8::unchecked::append(e.second, std::back_inserter(result));
            }
        }
        result.append(1, ']');
    }

    void operator()(const std::string& str) {
        auto it = str.begin();
        const auto end = str.end();
        while (it != end) {
            const auto it_start = it;
            const auto ch = utf8::unchecked::next(it);
            const bool escape = ch == U'*' || ch == U'?' || ch == U'[' || ch == U']';
            if (escape) {
                result.append(1, '[');
            }
            result.append(it_start, it);
            if (escape) {
                result.append(1, ']');
            }
        }
    }
};

std::string glob_stringify(const glob::element_vector& elements) {
    std::string result;

    stringify_visitor v(result);
    for (const auto& el: elements) {
        std::visit(v, el);
    }
    return result;
}

}