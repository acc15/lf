#include "io/openmode.hpp"

#include <vector>
#include <utility>

namespace lf {

    
    const std::vector<std::pair<std::ios_base::openmode, const char*>> ios_flag_names = {
        { std::ios_base::in, "in" }, // open for reading
        { std::ios_base::out, "out" }, // open for writing
        { std::ios_base::binary, "binary" }, // open in binary mode
        { std::ios_base::app, "app" }, // seek to the end of stream before each write
        { std::ios_base::trunc, "trunc" }, // discard the contents of the stream when opening
        { std::ios_base::ate, "ate" }, // seek to the end of stream immediately after open
        // { std::ios_base::noreplace, "open in exclusive mode" },
    };

    std::ostream& operator<<(std::ostream& s, with_format<openmode_format, const std::ios_base::openmode> flags) {
        bool need_sep = false;
        for (const auto& mode_pair: ios_flag_names) {
            if ((flags.value & mode_pair.first) == 0) {
                continue;
            }
            if (need_sep) {
                s << "|";
            }
            s << mode_pair.second;
            need_sep = true;
        }
        return s;
    }

}