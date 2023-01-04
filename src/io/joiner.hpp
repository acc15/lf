#include <ostream>
#include <ranges>
#include <functional>

namespace lf {

    template <typename Range, typename Mapper>
    struct joiner {
        joiner(std::string_view separator, const Range& range, Mapper mapper): separator(separator), range(range), mapper(mapper) {}
        std::string_view separator;
        const Range& range; 
        const Mapper mapper;
    };

    template <typename Range>
    joiner<Range, std::identity> join(std::string_view separator, const Range& range) {
        return joiner<Range, std::identity>(separator, range, {});
    }

    template <typename Range, typename Mapper>
    joiner<Range, Mapper> join(std::string_view separator, const Range& range, Mapper mapper) {
        return joiner<Range, Mapper>(separator, range, mapper);
    }

    template <typename Range, typename Mapper>
    std::ostream& operator<<(std::ostream& s, const joiner<Range, Mapper>& joiner) {
        auto it = std::begin(joiner.range);
        const auto end = std::end(joiner.range);
        if (it == end) {
            return s;
        }
        while (true) {
            s << joiner.mapper(*it);
            ++it;
            if (it == end) {
                break;
            }
            s << joiner.separator;
        }
        return s;
    }

}