#pragma once

#include <span>
#include <fmt/format.h>
namespace lf
{
    struct cmd_desc
    {
        std::span<const char *> names;
        const char *description;
    };
    
}

template <>
struct fmt::formatter<lf::cmd_desc> : fmt::formatter<std::string_view>
{
    template <typename FormatContext>
    auto format(const lf::cmd_desc &c, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "{} - {}", fmt::join(c.names, ", "), c.description);
    }
};


