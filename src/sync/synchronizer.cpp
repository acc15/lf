#include "sync/synchronizer.hpp"
#include "sync/entry_synchronizer.hpp"
#include "log/log.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace lf {

    synchronizer::synchronizer(const config::sync& sync, const lf::index& index, lf::tracked_state& state): 
        config(sync), 
        index(index), 
        state(state)
    {
    }

    void synchronizer::run() {
        queue = { sync_item { fs::path(), index.data, false } };
        while (!queue.empty()) {
            entry_synchronizer s(*this, queue.back());
            queue.pop_back();
            
            s.run();

            const fs::path& p = s.path();
            log.with(s.level()) && log() << (p.empty() ? "<root>" : p.string()) << ": " << s.message() << log::end;
        }
    }

}