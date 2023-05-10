#include <iostream>
#include <filesystem>

#include "cmd/impl/mv_cmd.hpp"

#include "log/log.hpp"

namespace fs = std::filesystem;

namespace lf {

    mv_cmd::mv_cmd() : cmd(
        { "m", "move", "mv" }, 
        "moves index node to another path",
        {
            opt { "force", 'F', "also moves file or directory" },
            opt { "", '\0', "path to source file or directory", "source", 1, 1 },
            opt { "", '\0', "path to target file or directory", "target", 1, 1 },
        }
    ) {
    }

    bool mv_cmd::run(cmd_context& ctx) const {
        const opt_map::vec_type& paths = ctx.opts[""];
        if (paths.size() % 2 != 0) {
            log.error() && log() << "invalid count of arguments - only path pairs are accepted" << log::end;
            return false;
        }

        bool ok = true;
        for (auto it = paths.begin(); it != paths.end();) {
            const auto from = *it;
            ++it;

            const auto to = *it;
            ++it;

            const auto move_pair = normalize_move(from, to);
            if (!move_pair) {
                ok = false;
                continue;
            }

            const auto from_pi = normalize_rel(move_pair->first, ctx.config.local);
            const auto to_pi = normalize_rel(move_pair->second, ctx.config.local);
            if (!from_pi || !to_pi) {
                ok = false;
                continue;
            }

            ok &= move(ctx, *from_pi, *to_pi);
        }
        return ok;
    }

    bool mv_cmd::move(cmd_context& ctx, const path_pair& from, const path_pair& to) const {
        // a/b/c -> a/b/c/d/e
        if (ctx.opts.has("force")) {
            move_path(from.first, to.first);
        }

        return false;
    }

}