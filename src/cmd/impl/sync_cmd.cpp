#include <iostream>

#include "cmd/impl/sync_cmd.hpp"
#include "config/config.hpp"
#include "io/log.hpp"
#include "io/joiner.hpp"

#include "sync/synchronizer.hpp"

namespace lf {

    sync_cmd::sync_cmd(): cmd(
        { "s", "sync" }, 
        "synchronizes mirrors", 
        { 
            opt { "", '\0', "configured names to synchronize", "syncs", 0, opt::UNBOUNDED }
        }
    ) {}

    int sync_cmd::run(const opt_map& opts) const {
        const config cfg = config::load();

        const config::sync_entry_vec syncs = cfg.find_name_matches(opts[""]);
        if (syncs.empty()) {
            log.error() && log() 
                << "no one sync found by supplied names, declared sync names: " 
                << join(", ", cfg.syncs, [](const auto& e) { return e.first; }) 
                << std::endl;
            return 1;
        }

        bool ok = true;
        for (const config::sync_entry* p: syncs) {
            ok &= do_sync(p->first, p->second);
        }
        return ok ? 0 : 1;
    }

    bool sync_cmd::do_sync(const std::string& name, const config::sync& sync) const {
        log.info() && log() 
            << "syncing \"" << name
            << "\", local: " << sync.local 
            << ", remote: " << sync.remote 
            << std::endl;

        synchronizer s(name, sync);
        try {
            s.load();
            s.run();
            s.save();
        } catch (const std::runtime_error& e) {
            log.error() && log() << "unable to sync \"" << name << "\": " << e.what() << std::endl;
            return false;
        }
        return true;
    }

}