#pragma once

#include <optional>

#include "glob/glob.hpp"

namespace lf {

std::string& glob_make_string_ref(glob::element_vector& els);

template <std::input_iterator Iter, std::sentinel_for<Iter> Sentinel>
static bool non_stop_parse(Iter, Sentinel) {
    return false;
}

template <std::input_iterator Iter, std::sentinel_for<Iter> Sentinel, std::invocable<Iter, Sentinel> StopFunction = bool(*)(Iter,Sentinel)>
static glob::element_vector glob_parse(Iter& it, Sentinel end, StopFunction stop = non_stop_parse) {
    glob::element_vector els;
    while (it != end) {
        if (stop(it, end)) {
            break;
        }

        const auto it_ch_begin = it;
        const auto ch = utf8::unchecked::next(it);
        switch (ch) {
        case U'*': 
            if (els.empty() || !std::holds_alternative<glob::star>(els.back())) {
                els.push_back(glob::star());
            }
            break;

        case U'?':
            els.push_back(glob::any());
            break;

        case U'[':
            glob_parse_range(it, end, els);
            break;

        default:
            glob_make_string_ref(els).append(it_ch_begin, it);
            break;
        }
    }
    return els;
}

template <std::input_iterator Iter, std::sentinel_for<Iter> Sentinel>
void glob_parse_range(Iter& it, Sentinel end, glob::element_vector& els) {
    
    glob::range range;

    std::optional<utf8::utfchar32_t> last_char = std::nullopt;
    if (it != end) {
        const auto first_char = utf8::unchecked::next(it);
        if (first_char == U'!') {
            range.inverse = true;
        } else {
            last_char = first_char;
        }
    }

    bool in_char_range = false;
    while (it != end) {
        const auto ch = utf8::unchecked::next(it);
        if (ch == U']') {
            break;
        }
        
        if (in_char_range) {
            range.add(last_char.value(), ch);
            last_char = std::nullopt;
            in_char_range = false;
            continue;
        }

        if (ch == U'-' && last_char.has_value()) {
            in_char_range = true;
            continue;
        }

        if (last_char.has_value()) {
            range.add(last_char.value());
        }
        
        last_char = ch;
    }

    if (last_char.has_value()) {
        range.add(last_char.value());
    }

    if (in_char_range) {
        range.add(U'-');
    }

    if (range.map.empty()) {
        if (range.inverse) {
            utf8::unchecked::append(U'!', std::back_insert_iterator(glob_make_string_ref(els)));
        }
        return;
    } 
    
    if (range.map.size() == 1) {
        const auto iter = range.map.begin();
        if (iter->first == iter->second) {
            utf8::unchecked::append(iter->first, std::back_insert_iterator(glob_make_string_ref(els)));
            return;
        }
    }

    els.push_back(std::move(range));
}


}