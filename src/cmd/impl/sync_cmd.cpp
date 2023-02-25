#include "fs/adv_fstream.hpp"
#include "fs/serialization.hpp"
#include "cmd/impl/sync_cmd.hpp"
#include "config/config.hpp"
#include "log/log.hpp"
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

    bool sync_cmd::run(const opt_map& params) const {
        const config cfg = config::load();

        const config::sync_entry_vec syncs = cfg.find_name_matches(params[""]);
        if (syncs.empty()) {
            log.error() && log() 
                << "no one sync found by supplied names, declared sync names: " 
                << join(", ", cfg.syncs, [](const auto& e) { return e.name; }) 
                << log::end;
            return 1;
        }

        bool ok = true;
        for (const config::sync* p: syncs) {
            ok &= do_sync(*p);
        }
        return ok;
    }

    bool sync_cmd::do_sync(const config::sync& sync) const {
        log.info() && log() 
            << "syncing \"" << sync.name
            << "\", local: " << sync.local 
            << ", remote: " << sync.remote 
            << log::end;

        try {
            do_sync_safe(sync);
        } catch (const std::runtime_error& e) {
            log.error() && log() << "unable to sync \"" << sync.name << "\": " << e.what() << log::end;
            return false;
        }
        return true;
    }

    void sync_cmd::do_sync_safe(const config::sync& sync) const {
        
        index index;

        adv_fstream index_stream;
        if (open_and_lock<tree_binary_format, lf::index>(sync.index, index_stream, OPEN_READ_LOCK)) {
            load_file<tree_binary_format>(sync.index, index_stream, index);
        }

        tracked_state state;
        adv_fstream state_stream;

        open_and_lock<tree_binary_format, lf::state>(sync.state, state_stream, OPEN_READ_WRITE_LOCK);
        if (state_stream.tellg() > 0) { // file not empty
            state_stream.seekg(0);
            load_file<tree_binary_format>(sync.state, state_stream, state.root);
        }

        synchronizer s(sync, index, state);
        s.run();

        if (state.changed) {
            state_stream.truncate();
            save_file<tree_binary_format>(sync.state, state_stream, state.root);
        }

    }



}