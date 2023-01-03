#include <iostream>

#include "cmd/impl/sync_cmd.hpp"
#include "config/config.hpp"
#include "io/log.hpp"
#include "io/joiner.hpp"

#include "sync/synchronizer.hpp"

namespace fs = std::filesystem;

namespace lf {

    sync_cmd::sync_cmd(): cmd({ "s", "sync" },  "[list of sync names, all if empty]+", "synchronizes mirrors") {
    }

    int sync_cmd::run([[maybe_unused]] const std::span<const char*>& args) const {
        const config cfg = config::load();

        const config::sync_entry_vec syncs = cfg.find_name_matches(args);
        if (syncs.empty()) {
            log.error() && log() 
                << "no one sync found by supplied names, declared sync names: " 
                << join(", ", cfg.syncs, [](const auto& e) { return e.first; }) 
                << std::endl;
            return 1;
        }

        for (const config::sync_entry* p: syncs) {
            synchronizer s(p->first, p->second);
            try {
                if (s.init()) {
                    s.run();
                }
            } catch (const std::runtime_error& e) {
                log.error() && log() << "unable to sync \"" << p->first << "\": " << e.what() << std::endl;
            }
        }

        return 0;
    }

}