#pragma once

#include <vector>
#include <string>

namespace lf {

enum class glob_element_type {
    PLAIN,
    GLOB,
    GLOBSTAR
};

struct glob_element {
    glob_element_type type;
    std::string value;
};

class glob_path {

public:
    std::vector<glob_element> elements;

};

}