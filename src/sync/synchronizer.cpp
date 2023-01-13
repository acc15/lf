#include "sync/synchronizer.hpp"
#include "io/log.hpp"
#include "fs/serialization.hpp"
#include "tree/binary.hpp"

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
        queue = { queue_item { .path = fs::path(), .mode = index.data } };
        while (!queue.empty()) {
            const queue_item& item = queue.back();
            handle(item, path_info(sync.local / item.path), path_info(sync.remote / item.path));
            queue.pop_back();
        }
    }

    void synchronizer::handle(const queue_item& item, const path_info& l, const path_info& r) {
        if (l.type == fs::file_type::not_found && r.type == fs::file_type::not_found) {
            log.info() && log() << item.path << ": no file or directory exists on local and remote sides" << std::endl;
            index.remove(item.path);
            state.remove(item.path);
            return;
        }


    }

    void synchronizer::load() {
        
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

    }

    void synchronizer::save() {
        save_file(sync.index, index);
        save_file(sync.state, state);
    }

}