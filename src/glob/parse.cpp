#include "glob/parse.hpp"

namespace lf {

std::string& glob_make_string_ref(glob::element_vector& els) {
    if (els.empty() || !std::holds_alternative<std::string>(els.back())) {
        els.push_back(std::string());
    }
    return std::get<std::string>(els.back());
}

}