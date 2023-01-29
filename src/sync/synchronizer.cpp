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
            const sync_item item = std::move(queue.back());
            queue.pop_back();

            entry_synchronizer s(*this, item);
            s.run();

            log.with(s.level()) && log() 
                << (item.path.empty() ? "<root>" : item.path.string()) << ": " << s.message() 
                << log::end;
        }
    }

}