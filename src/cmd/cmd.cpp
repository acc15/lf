#include <fmt/core.h>
#include <fmt/ranges.h>

#include "cmd.hpp"

namespace leafsync {

cmd::cmd(const std::span<const char*>& names, const char* description): names(names), description(description) {
}

std::string cmd::usage() const {
    return fmt::format("{}", fmt::join(names, ", ")) + " - " + description;
}

}