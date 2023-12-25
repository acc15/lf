#include <catch2/catch_test_macros.hpp>

#include <filesystem>

#include "glob/glob_path.hpp"
#include "fs/time.hpp"

TEST_CASE("test", "[glob_sync]") {
    
}

namespace fs = std::filesystem;

enum sync_mode {
    NONE,
    INCLUDE,
    EXCLUDE
};

struct index_node {
    sync_mode mode;
    std::map<lf::glob_path::element, index_node> items;
};

struct state_node {
    bool synced;
    std::map<std::string, state_node> items;
};

struct state_view {
    state_node* last_known;
    std::vector<std::string> path;

    state_view next(const std::string& el) const {
        std::vector<std::string> new_path = path;
        new_path.push_back(el);
        return state_view { last_known, std::move(new_path) };
    }

    bool get() {
        for (const std::string& e: path) {
            const auto it = last_known->items.find(e);
            if (it == last_known->items.end()) {
                return false;
            }
            last_known = &(it->second);
        }
        return last_known->synced;
    }

    void set(bool value) {
        for (const std::string& e: path) {
            last_known = &(last_known->items[e]);
        }
        path.clear();
        last_known->synced = value;
    }
};

struct path_info {
    bool left;
    fs::path path;
    fs::file_type type;

    const char* name() const {
        return left ? "left" : "right";
    }

    bool dir() const {
        return type == fs::file_type::directory;
    }

    bool exists() const {
        return type != fs::file_type::not_found;
    }

    bool plain() const {
        return type != fs::file_type::directory && type != fs::file_type::not_found;
    }

    fs::file_time_type time() const {
        return lf::ntfs_last_write_time(path);
    }
};

struct sync_item {
    fs::path path;
    std::vector<index_node*> matches;
    state_view state;
};


class glob_entry_sync {

};

class glob_sync {

    

};

// lf::glob_path::element

void sync(sync_item& item) {

}