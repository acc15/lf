#include "io/file_type.hpp"

using std::filesystem::file_type;

namespace lf {

    const std::unordered_map<std::filesystem::file_type, const char*> file_type_names = {
        { file_type::none, "none" },
        { file_type::unknown, "unknown" },
        { file_type::block, "block file" },
        { file_type::character, "character file" },
        { file_type::symlink, "symlink" },
        { file_type::directory, "directory" },
        { file_type::regular, "regular file" },
        { file_type::fifo, "pipe file" },
        { file_type::socket, "socket file" }
    };

    std::ostream& operator<<(std::ostream& s, const file_type& t) {
        const auto it = file_type_names.find(t);
        if (it == file_type_names.end()) {
            s << file_type_names.at(file_type::unknown) << " (code " << static_cast<unsigned int>(t) << ")";
        } else {
            s << it->second;
        }
        return s;
    }

}