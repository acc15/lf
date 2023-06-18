#pragma once

#include <istream>
#include <vector>
#include <memory>
#include <algorithm>

namespace lf {

    
    template <typename EncodingTraits> 
    struct glob {

        using encoding = EncodingTraits;
        using char_type = typename encoding::char_type;
        using char_traits = typename encoding::char_traits;
        using streambuf = std::basic_streambuf<char_type, char_traits>;
        using istream = std::basic_istream<char_type, char_traits>;
        using istreambuf_iterator = std::istreambuf_iterator<char_type, char_traits>;
        using string = typename encoding::string;
        using string_view = typename encoding::string_view;
        using codepoint = typename encoding::codepoint;

        class codepoint_predicate {
        public:
            virtual bool matches(const codepoint& cp) const = 0;
        };

        class range_predicate: public codepoint_predicate {
            codepoint min;
            codepoint max;

        public:
            range_predicate(const codepoint& min, const codepoint& max): min(min), max(max) {
            }

            bool matches(const codepoint& cp) const override {
                return cp >= min && cp <= max;
            }
        };

        class or_predicate: public codepoint_predicate {
            std::vector<std::unique_ptr<const codepoint_predicate>> predicates;
        public:
            bool matches(const codepoint& cp) const override {
                return std::any_of(
                    predicates.begin(), 
                    predicates.end(), 
                    [&cp](const codepoint_predicate& p) { return p.matches(cp); }
                );
            }
        };

        class any_predicate: public codepoint_predicate {
        public:
            bool matches(const codepoint&) const override {
                return true;
            }
        };

        class not_predicate: public codepoint_predicate {
            std::unique_ptr<const codepoint_predicate> predicate;
        public:
            bool matches(const codepoint& cp) const override {
                return !predicate.matches(cp);
            }
        };

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