#include <ostream>
#include <ranges>

namespace lf {

    template <typename Range>
    struct joiner {
        joiner(const Range& range, std::string_view separator): range(range), separator(separator) {}
        const Range& range; 
        std::string_view separator;
    };

    template <typename Range>
    std::ostream& operator<<(std::ostream& s, const joiner<Range>& joiner) {
        auto it = std::begin(joiner.range);
        const auto end = std::end(joiner.range);
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