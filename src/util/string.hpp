#pragma once

#include <algorithm>
#include <locale>
#include <string_view>

namespace leafsync {

	template <std::input_iterator I, std::sentinel_for<I> S>
	size_t count_first_spaces(const I first, const S last) {
		return std::distance(first, std::find_if_not(first, last, [](const auto& ch) { return ch >= 0 && std::isspace(ch); }));
	}

	std::string_view rtrim(const std::string_view& str);
	std::string_view ltrim(const std::string_view& str);
	std::string_view trim(const std::string_view& str);

}