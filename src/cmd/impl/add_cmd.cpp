#include "cmd/impl/add_cmd.hpp"

namespace fs = std::filesystem;

namespace lf {

    add_cmd::add_cmd() : cmd(
        { "a", "add", "set" }, 
        "adds specified file and directories to corresponding index files",
        {
            { "shallow", 'S', "add paths with SHALLOW mode", "paths", 0, opt::UNBOUNDED },
            { "recursive", 'R', "add paths with RECURSIVE mode", "dirpaths", 0, opt::UNBOUNDED },
            { "index", 'I', "don't check paths, just update index" },
            { "", '\0', "same as --shallow", "paths", 0, opt::UNBOUNDED },
        },
        "shallow"
    ) {
    }

    bool add_cmd::run(cmd_context& ctx) const {
        return add_paths(ctx, sync_mode::SHALLOW, "shallow") 
            && add_paths(ctx, sync_mode::RECURSIVE, "recursive");
    }

    bool add_cmd::add_paths(cmd_context& ctx, sync_mode mode, const char* opt_name) {
        bool ok = true;
        for (const auto p: ctx.opts[opt_name]) {
            ok &= add_path(ctx, mode, p);
        }
        return ok;
    }

    bool add_cmd::add_path(cmd_context& ctx, sync_mode mode, std::string_view path) {
        const auto pi = normalize_rel(path, ctx.config.left);
        if (!pi) {
            return false;
        }
        if (!ctx.opts.has("index")) {
            if (mode == sync_mode::RECURSIVE && !std::filesystem::is_directory(pi->first)) {
                log.error() && log() << "path " << pi->first << " isn't a directory" << log::end;
                return false;
            }
            if (mode == sync_mode::SHALLOW && !fs::exists(pi->first)) {
                log.error() && log() << "path " << pi->first << " doesn't exists" << log::end;
                return false;
            }
        }
        ctx.index->set(pi->second, mode);
        return true;
    }

}