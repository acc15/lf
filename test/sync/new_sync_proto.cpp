#include <filesystem>
#include <vector>
#include <map>
#include <string>
#include <variant>
#include <unordered_map>
#include <algorithm>

#include "fs/util.hpp"

namespace lf {

    struct state {
        bool value;
        std::map<std::string, state> children;
    };

    struct glob {};
    struct globstar {};

    enum class sync_mode {
        NONE,
        INCLUDE,
        EXCLUDE
    };

    struct index {
        using key_type = std::variant<std::string, glob, globstar>;

        sync_mode value;
        std::map<key_type, index> children;
    };

    struct sync_pair {
        std::filesystem::path path;
        std::filesystem::directory_entry left;
        std::filesystem::directory_entry right;
        state* db;
    };


    class entry_provider {
    public:
        virtual std::optional<sync_pair> next() = 0;
    };

    class simple_path_provider: public entry_provider {

        std::filesystem::path p;
        std::filesystem::path l;
        std::filesystem::path r;

        sync_pair i;
        bool done = false;

        state db;

    public:

        std::optional<sync_pair> next() override {
            if (done) {
                return std::nullopt;
            }
            done = true;
            return sync_pair {
                .path = p,
                .left = std::filesystem::directory_entry(lf::join_path(l, p)),
                .right = std::filesystem::directory_entry(lf::join_path(r, p)),
                .db = &db
            };
        }

    };

    class index_path_provider: public entry_provider {
    public:
        using path = std::filesystem::path;

        index&  patterns;
        path    left;
        path    right;
        path    cur_path;

        std::unordered_map<path, std::vector<index*>> cached_matches;

        index_path_provider(index& i, const path& l, const path& r): patterns(i), left(l), right(r), cur_path() {
            cached_matches[cur_path].push_back(&patterns);
        }

        std::optional<sync_pair> next() override {

            const std::vector<index*>& matches = cached_matches[cur_path];
            if (std::ranges::any_of(matches, [](auto i) { return i->value == sync_mode::EXCLUDE; })) {
                cached_matches.erase(cur_path);
                if (cur_path.empty()) {
                    return std::nullopt;
                } else {
                    cur_path = cur_path.parent_path();
                }
            }
            return std::nullopt;
        }

    };



    class item_syncer {
        virtual void sync(const sync_pair& item) = 0;
    };

    class entry_comparator {
        using entry = std::filesystem::directory_entry;
        virtual int compare(const entry& l, const entry& r) = 0;
    };

}