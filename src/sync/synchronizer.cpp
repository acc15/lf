#include "sync/synchronizer.hpp"
#include "sync/entry_synchronizer.hpp"
#include "log/log.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace lf {

    synchronizer::synchronizer(const config::sync& sync): config(sync) {
    }

    void synchronizer::run() {
        queue = { sync_item { fs::path(), index.get(), false } };
        while (!queue.empty()) {
            entry_synchronizer s(*this);
            s.run();

            const fs::path& p = s.path();
            log.with(s.level()) && log() << (p.empty() ? "<root>" : p.string()) << ": " << s.message() << log::end;
        }
    }

    void synchronizer::load() {
        try {
            index.load(config.index);
        } catch (const file_not_found_error& e) {
            log.warn() && log() << e.what() << log::end;
        }
        try {
            state.load(config.state);
        } catch (const file_not_found_error& e) {
            log.debug() && log() << e.what() << log::end;
        }
    }

    void synchronizer::save() {
        index.save_if_changed(config.index);
        state.save_if_changed(config.state);
    }

}