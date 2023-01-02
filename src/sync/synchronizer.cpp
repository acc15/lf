#include "sync/synchronizer.hpp"
#include "io/log.hpp"
#include "fs/serialization.hpp"
#include "tree/tree_binary.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace lf {

    synchronizer::synchronizer(const config::sync_entry& sync): sync(sync) {
    }

    void synchronizer::run() {
        if (!init()) {
            return;
        }

    }

    bool synchronizer::init() {

        log.info() && log() 
            << "syncing \"" << sync.first 
            << "\", local: " << sync.second.local 
            << ", remote: " << sync.second.remote 
            << std::endl;
        
        if (!fs::exists(sync.second.local)) {
            log.error() && log() << "local path of sync \"" << sync.first << "\" doesn't exists" << std::endl;
            return false;
        }

        if (!fs::exists(sync.second.remote)) {
            log.error() && log() << "remote path of sync \"" << sync.first << "\" doesn't exists" << std::endl;
            return false;
        }

        index_tree index;
        try {
            load_file(sync.second.index, index);
        } catch (const file_not_found_error& e) {
            log.warn() && log() << e.what() << std::endl;
        }

        state_tree state;
        try {
            load_file(sync.second.state, state);
        } catch (const file_not_found_error& e) {
            log.debug() && log() << e.what() << std::endl;
        }

        if (index.data == sync_mode::NONE && index.entries.empty()) {
            log.info() && log() << "index of sync \"" << sync.first << "\" is empty, nothing to sync" << std::endl;
            return false;
        }
        
        return true;
    }

}