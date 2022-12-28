#include <ostream>
#include <ranges>

namespace lf {

    template <std::ranges::range Range>
    struct joiner {
        joiner(const Range& range, std::string_view separator): range(range), separator(separator) {}
        const Range& range; 
        std::string_view separator;
    };

    template <std::ranges::range Range>
    std::ostream& operator<<(std::ostream& s, const joiner<Range>& joiner) {
        auto it = joiner.range.begin();
        const auto end = joiner.range.end();
        if (it == end) {
            return s;
        }
        while (true) {
            s << *it;
            ++it;
            if (it == end) {
                break;
            }
            s << joiner.separator;
        }
        return s;
    }

}