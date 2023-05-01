#include "cmd/impl/mv_cmd.hpp"

namespace lf {

    mv_cmd::mv_cmd() : cmd(
        { "m", "move", "mv" }, 
        "moves file or directory to specified path (and updates index files)",
        {
            opt { "", '\0', "path to source file or directory", "source", 1, 1 },
            opt { "", '\0', "path to target file or directory", "target", 1, 1 },
        }
    ) {
    }

    bool mv_cmd::run(const opt_map& params) const {
        const opt_map::vec_type& paths = params[""];
        if (paths.size() % 2 != 0) {

        }
        return true;
    }

}