#pragma once

#include <iostream>

namespace lf {

    template <typename Stream>
    concept iostream_type = requires (Stream& s) {
        []<typename CharType, typename TraitsType>(std::basic_iostream<CharType, TraitsType>&){}(s);
    };

    template <typename Stream>
    concept istream_type = !iostream_type<Stream> && requires (Stream& s) {
        []<typename CharType, typename TraitsType>(std::basic_istream<CharType, TraitsType>&){}(s);
    };

    template <typename Stream>
    concept ostream_type = !iostream_type<Stream> && requires (Stream& s) {
        []<typename CharType, typename TraitsType>(std::basic_ostream<CharType, TraitsType>&){}(s);
    };

}