#include "glob/glob.hpp"

namespace lf {

glob::glob(matcher_vector&& matchers): matchers(std::move(matchers)) {}

bool glob::matches(std::streambuf* buf) const {
    std::vector<repetition_info> rep;

    size_t i = 0;
    while (i < matchers.size()) {
        const glob_matcher& m = *matchers[i];

        size_t repetition = 0;
        if (m.is_repetitive()) {
            if (!rep.empty() && rep.back().index == i) {
                repetition = rep.back().repetition;
            } else {
                std::streampos pos = buf->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
                rep.push_back(repetition_info { pos, i, repetition });
            }
        }
        
        const bool is_last = i == (matchers.size() - 1);
        if (m.matches(buf, repetition, is_last)) {
            ++i;
            continue;
        }

        if (rep.empty()) {
            return false;
        }

        if (m.is_repetitive()) {
            rep.pop_back();
        }

        repetition_info& r = rep.back();
        buf->pubseekpos(r.position, std::ios_base::in);
        i = r.index;
        ++r.repetition;

    }
    return buf->sgetc() == std::char_traits<char>::eof();
}

glob glob::parse(std::string_view view) {
    return glob();
}

}