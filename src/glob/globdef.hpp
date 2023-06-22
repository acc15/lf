#pragma once

#include <istream>
#include <vector>
#include <memory>

#include "../encoding/encoding.hpp"

namespace lf {

    template <encoding_type Encoding> 
    class glob {
    public:
        using encoding = Encoding;
        using char_type = typename encoding::char_type;
        using char_traits = typename encoding::char_traits;
        using string = typename encoding::string;
        using string_view = typename encoding::string_view;
        using codepoint = typename encoding::codepoint;

        using streambuf = std::basic_streambuf<char_type, char_traits>;
        using istreambuf_iterator = std::istreambuf_iterator<char_type, char_traits>;
        using istream = std::basic_istream<char_type, char_traits>;

        using pos_type = typename char_traits::pos_type;

        class matcher {
        public:
            virtual bool is_repetitive() const {
                return false;
            }

            virtual bool matches(streambuf* buf, size_t repetition, bool is_last) const = 0;
        };

        class star_matcher;
        class char_matcher;
        class string_matcher;

        using matcher_ptr = std::unique_ptr<const matcher>;
        using matcher_vector = std::vector<matcher_ptr>;

    private:
        matcher_vector matchers;

        struct repetition_info {
            pos_type position;
            size_t index;
            size_t repetition;
        };

    public:
        glob(matcher_vector&& matchers = {}): matchers(std::move(matchers)) {}

        bool matches(streambuf* buf) const {
            std::vector<repetition_info> rep;

            size_t i = 0;
            while (i < matchers.size()) {
                const matcher& m = *matchers[i];

                size_t repetition = 0;
                if (m.is_repetitive()) {
                    if (!rep.empty() && rep.back().index == i) {
                        repetition = rep.back().repetition;
                    } else {
                        pos_type pos = buf->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
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
            return buf->sgetc() == char_traits::eof();
        }

        static glob parse(std::string_view view) {
            return glob();
        }

    };

}