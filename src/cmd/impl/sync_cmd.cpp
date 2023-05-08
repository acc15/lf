#include "io/adv_fstream.hpp"
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
        {}
    ) {}

    bool sync_cmd::run(const opt_map&) const {
        const config cfg = config::load();
        return do_sync(cfg);
    }

    bool sync_cmd::do_sync(const config& config) const {
        log.info() && log() << "syncing, local: " << config.local << ", remote: " << config.remote << log::end;

        try {
            do_sync_safe(config);
        } catch (const std::runtime_error& e) {
            log.error() && log() << "unable to sync: " << e.what() << log::end;
            return false;
        }
        return true;
    }

    void sync_cmd::do_sync_safe(const config& config) const {
        
        index index;

        adv_fstream index_stream;
        if (open_and_lock<tree_binary_format, lf::index>(config.index, index_stream, OPEN_READ_LOCK)) {
            load_file<tree_binary_format>(config.index, index_stream, index);
        }

        tracked_state state;
        adv_fstream state_stream;

        open_and_lock<tree_binary_format, lf::state>(config.state, state_stream, OPEN_READ_WRITE_LOCK);
        if (state_stream.tellg() > 0) { // file not empty
            state_stream.seekg(0);
            load_file<tree_binary_format>(config.state, state_stream, state.root);
        }

        synchronizer s(config, index, state);
        s.run();

        if (state.changed) {
            state_stream.truncate();
            save_file<tree_binary_format>(config.state, state_stream, state.root);
        }

    }



}