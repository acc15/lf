#include <iostream>

#include "cmd/sync/sync_cmd.hpp"
#include "config/config.hpp"
#include "io/log.hpp"
#include "io/joiner.hpp"
#include "fs/serialization.hpp"
#include "index/index_tree.hpp"
#include "state/state_tree.hpp"
#include "tree/tree_binary.hpp"

namespace fs = std::filesystem;

namespace lf {

    void sync(config::sync_entry_ptr sync) {

        if (!fs::exists(sync->second.local)) {
            log.error() && log() << "local path of sync \"" << sync->first << "\" doesn't exists" << std::endl;
            return;
        }

        if (!fs::exists(sync->second.remote)) {
            log.error() && log() << "remote path of sync \"" << sync->first << "\" doesn't exists" << std::endl;
            return;
        }

        index_tree index;
        try {
            load_file(sync->second.index, index);
        } catch (const std::runtime_error& e) {
            log.warn() && log() << e.what() << std::endl;
        }

        state_tree state;
        try {
            load_file(sync->second.state, state);
        } catch (const std::runtime_error& e) {
            log.debug() && log() << e.what() << std::endl;
        }

        if (index.data == sync_mode::NONE && index.entries.empty()) {
            log.info() && log() << "index of sync \"" << sync->first << "\" is empty, nothing to sync" << std::endl;
            return;
        }

        // TODO

    }

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

        for (config::sync_entry_ptr p: syncs) {
            log.info() && log() << "syncing: " << p->first << ", local: " << p->second.local << ", remote: " << p->second.remote << std::endl;
            sync(p);
        }

        return 0;
    }

}