#include <iostream>
#include <filesystem>

#include "cmd/impl/mv_cmd.hpp"

#include "log/log.hpp"
#include "index/indexer.hpp"

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

    bool mv_cmd::run(const opt_map& params) const {
        const opt_map::vec_type& paths = params[""];
        if (paths.size() % 2 != 0) {
            log.error() && log() << "invalid count of arguments - only path pairs are accepted" << log::end;
            return false;
        }

        indexer i;
        for (auto it = paths.begin(); it != paths.end();) {
            const auto from = i.resolve(*it);
            ++it;
            const auto to = i.resolve(*it);
            ++it;
            if (!from.has_value() || !to.has_value()) {
                continue;
            }
            // TODO implement
        }

        return true;
    }

}