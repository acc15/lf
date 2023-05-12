#include <iostream>
#include <filesystem>

#include "cmd/impl/mv_cmd.hpp"

#include "log/log.hpp"

namespace lf {

    mv_cmd::mv_cmd() : cmd(
        { "m", "move", "mv" }, 
        "moves file/directory to another path",
        {
            opt { "index", 'I', "don't move files - update index only" },
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
        const bool move_file = !ctx.opts.has("index");
        for (auto it = paths.begin(); it != paths.end();) {
            const auto from = *it;
            ++it;

            const auto to = *it;
            ++it;

            ok &= move(ctx, from, to, move_file);
        }
        return ok;
    }

    bool mv_cmd::move(cmd_context& ctx, std::string_view from, std::string_view to, bool move_file) const {
        
        const auto move_pair = check_move(from, to, move_file);
        if (!move_pair) {
            return false;
        }

        const auto from_pi = normalize_rel(move_pair->first, ctx.config.local);
        const auto to_pi = normalize_rel(move_pair->second, ctx.config.local);
        if (!from_pi || !to_pi) {
            return false;
        } 

        if (move_file) {
            try {
                move_path(from_pi->first, to_pi->first);
            } catch (const std::runtime_error& e) {
                log.error() && log() 
                    << "can't move " << from_pi->first 
                    << " to " << to_pi->first 
                    << ": " << e.what() 
                    << log::end;
                return false;
            }
        }
        ctx.index->move(from_pi->second, to_pi->second);
        return true;
    }

}