#pragma once

#include <concepts>
#include <vector>
#include <map>
#include <variant>
#include <utf8/unchecked.h>

namespace lf {

    class glob {
    public:
        struct any {};
        struct star {};
        struct range {
            using map_type = std::map<utf8::utfchar32_t, utf8::utfchar32_t>;

            map_type map;
            bool inverse;

            range();
            range(const range& cp);
            range(range&& mv);
            range(const std::initializer_list<std::pair<const utf8::utfchar32_t, utf8::utfchar32_t>>& map_init);
        };

        template <typename T>
        static bool is_star(const T& v) {
            return std::holds_alternative<star>(v);
        }

        using element = std::variant<std::string, range, any, star>;
        using element_vector = std::vector<element>;

        element_vector elements;

        glob() = default;
        glob(const element_vector& v);
        glob(element_vector&& v);
        glob(const std::initializer_list<element>& v);

        bool matches(std::string_view sv) const;

        template <std::input_iterator Iter, std::sentinel_for<Iter> Sentinel>
        static bool non_stop_parse(Iter, Sentinel) {
            return false;
        }

        template <std::input_iterator Iter, std::sentinel_for<Iter> Sentinel, std::invocable<Iter, Sentinel> StopFunction = bool(*)(Iter,Sentinel)>
        static glob parse(Iter& it, Sentinel end, StopFunction fn = non_stop_parse) {
            glob::element_vector els;

            bool in_range = false;
            while (it != end) {
                if (in_range) {

                    glob::range& range = std::get<glob::range>(els.back());
                    const auto ch = utf8::unchecked::next(it);
                    in_range = false;

                }
                
                if (fn(it, end)) {
                    break;
                }

                const auto it_ch_begin = it;
                const auto ch = utf8::unchecked::next(it);
                switch (ch) {
                case U'*': 
                    els.push_back(glob::star());
                    break;

                case U'?':
                    els.push_back(glob::any());
                    break;

                case U'[':
                    els.push_back(glob::range());
                    in_range = true;
                    break;

                default:
                    if (!els.empty() && std::holds_alternative<std::string>(els.back())) {
                        std::get<std::string>(els.back()).append(it_ch_begin, it);
                    } else {
                        els.push_back(std::string(it_ch_begin, it));
                    }
                    break;
                }

            }

            return glob(std::move(els));
        }

        static glob parse(std::string_view str);

    };

}