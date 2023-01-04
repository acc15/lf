#include "sync/synchronizer.hpp"
#include "io/log.hpp"
#include "fs/serialization.hpp"
#include "tree/tree_binary.hpp"

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace lf {


    synchronizer::path_info::path_info(const std::filesystem::path& path): 
        path(path), 
        status(fs::status(path)), 
        type(status.type()) 
    {
    }

    synchronizer::synchronizer(const std::string& name, const config::sync& sync): name(name), sync(sync) {
    }

    void synchronizer::run() {
        while (!queue.empty()) {

            const std::pair<fs::path, sync_mode>& item = queue.back();
            const fs::path& path = item.first; 
            const sync_mode mode = item.second;

            const path_info l(sync.local / path);
            const path_info r(sync.remote / path);

            if (l.type == fs::file_type::not_found && r.type == fs::file_type::not_found) {
                log.info() && log() << path << ": no file or directory exists in local and remote side" << std::endl;
                continue;
            }

            if (r.type == fs::file_type::not_found) {
                
                if (state.get(path)) {
                    log.info() && log() << path << ": deleting from local side as sync flag set and no entries on remote side" << std::endl;
                    fs::remove_all(l.path);
                    state.set(path, false);
                    continue;
                }

                if (l.type == fs::file_type::directory) {
                    
                }

            }

            queue.pop_back();

        }
    }

    bool synchronizer::init() {
        queue.clear();

        log.info() && log() 
            << "syncing \"" << name 
            << "\", local: " << sync.local 
            << ", remote: " << sync.remote 
            << std::endl;
        
        try {
            load_file(sync.index, index);
        } catch (const file_not_found_error& e) {
            log.warn() && log() << e.what() << std::endl;
        }

        try {
            load_file(sync.state, state);
        } catch (const file_not_found_error& e) {
            log.debug() && log() << e.what() << std::endl;
        }
        
        queue.push_back(std::make_pair(fs::path(), index.data));
        return true;
    }

}