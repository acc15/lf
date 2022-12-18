#pragma once

#include <span>
#include <fmt/format.h>
namespace leafsync
{
struct cmd_desc
{
    std::span<const char *> names;
    const char *description;
};

}
template <>
struct fmt::formatter<leafsync::cmd_desc> : fmt::formatter<string_view>
{
    template <typename FormatContext>
    auto format(const leafsync::cmd_desc &c, FormatContext &ctx) const
    {
        return fmt::format_to(ctx.out(), "{} - {}", fmt::join(c.names, ", "), c.description);
    }
};
