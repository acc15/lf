#include "fs/serialization.hpp"

namespace lf {

    std::system_error create_system_error(const std::string& message) {
        std::error_code code(errno, std::generic_category());
        return std::system_error(code, message);
    }

}