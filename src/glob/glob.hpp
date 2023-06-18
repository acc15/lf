#pragma once

#include <istream>
#include <vector>
#include <memory>
#include <algorithm>

#include "predicate/dynamic_predicate.hpp"
#include "predicate/set_predicate.hpp"
#include "predicate/range_predicate.hpp"
#include "predicate/or_predicate.hpp"
#include "predicate/not_predicate.hpp"
#include "predicate/any_predicate.hpp"

namespace lf {

    template <typename Encoding> 
    struct glob {

        using encoding = Encoding;
        using char_type = typename encoding::char_type;
        using char_traits = typename encoding::char_traits;
        using string = typename encoding::string;
        using string_view = typename encoding::string_view;
        using codepoint = typename encoding::codepoint;

        using streambuf = std::basic_streambuf<char_type, char_traits>;
        using istreambuf_iterator = std::istreambuf_iterator<char_type, char_traits>;
        using istream = std::basic_istream<char_type, char_traits>;

        using codepoint_predicate = dynamic_predicate<codepoint>;
        using predicate_ptr = dynamic_predicate_ptr<codepoint>;
        using predicate_vector = std::vector<predicate_ptr>;
        using set_predicate = lf::set_predicate<codepoint>;
        using range_predicate = lf::range_predicate<codepoint>;
        using or_predicate = lf::or_predicate<codepoint>;
        using not_predicate = lf::not_predicate<codepoint>;
        using any_predicate = lf::any_predicate<codepoint>;

        class matcher {
        public:
            virtual bool is_repetitive() const {
                return false;
            }

            virtual bool matches(streambuf& buf, size_t repetition, bool is_last) const = 0;
        };

        class star_matcher: public matcher {
        public:
            bool is_repetitive() const override {
                return true;
            }

            bool matches(streambuf& buf, size_t repetition, bool is_last) const override {
                if (is_last) {
                    buf.pubseekoff(0, std::ios_base::end); // consume all chars past the end of stream
                    return true;
                }
                return buf.pubseekoff(repetition, std::ios_base::cur) != -1;
            }
        };

        class codepoint_matcher: public matcher {
            std::unique_ptr<codepoint_predicate> predicate;
        public:
            bool matches(streambuf& buf, size_t, bool) const override {
                istreambuf_iterator iter(buf), end;
                return iter != end && predicate->matches(encoding::next(iter, end));
            }
        };

        class string_matcher: public matcher {
            string str;
        public:
            bool matches(streambuf& buf, size_t, bool) const override {
                return std::mismatch(
                    str.begin(), str.end(), 
                    istreambuf_iterator(buf), istreambuf_iterator()
                ).first == str.end();
            }
        };

    };

}