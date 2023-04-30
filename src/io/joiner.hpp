#include <ostream>
#include <functional>

namespace lf {

    template <
        std::input_iterator It, 
        std::sentinel_for<It> EndIt,
        std::invocable<decltype(*std::declval<It>())> Mapper
    >
    struct joiner {
        joiner(std::string_view separator, const It& begin, const EndIt& end, Mapper mapper): 
            separator(separator), 
            begin(begin),
            end(end),
            mapper(mapper) 
        {
        }

        std::string_view separator;
        const It begin; 
        const EndIt end; 
        const Mapper mapper;
    };

    template <typename Range>
    auto join(std::string_view separator, const Range& range) {
        return joiner(separator, std::begin(range), std::end(range), std::identity {});
    }

    template <
        typename Range, 
        std::invocable<decltype(*std::begin(std::declval<Range>()))> Mapper
    >
    auto join(std::string_view separator, const Range& range, Mapper mapper) {
        return joiner(separator, std::begin(range), std::end(range), mapper);
    } 

    template <
        std::input_iterator It, 
        std::sentinel_for<It> EndIt, 
        std::invocable<decltype(*std::declval<It>())> Mapper
    >
    auto join(std::string_view separator, It begin, EndIt end, Mapper mapper) {
        return joiner(separator, begin, end, mapper);
    }

    template <
        std::input_iterator It, 
        std::sentinel_for<It> EndIt, 
        std::invocable<decltype(*std::declval<It>())> Mapper
    >
    std::ostream& operator<<(std::ostream& s, const joiner<It, EndIt, Mapper>& joiner) {
        auto it = joiner.begin;
        if (it == joiner.end) {
            return s;
        }
        while (true) {
            s << joiner.mapper(*it);
            ++it;
            if (it == joiner.end) {
                break;
            }
            s << joiner.separator;
        }
        return s;
    }

} 