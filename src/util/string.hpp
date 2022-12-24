#pragma once

#include <algorithm>
#include <locale>
#include <string_view>

namespace lf {

	bool is_ascii(char c);

	template <std::input_iterator I, std::sentinel_for<I> S>
	size_t count_first_spaces(const I first, const S last) {
		return std::distance(first, std::find_if_not(first, last, [](const auto& ch) { return is_ascii(ch) && std::isspace(ch); }));
	}

	std::string_view rtrim(std::string_view str);
	std::string_view ltrim(std::string_view str);
	std::string_view trim(std::string_view str);

	template <typename UnaryOp>
	std::string transform_ascii(std::string_view str, UnaryOp op) {
		std::string result;
		std::transform(str.begin(), str.end(), std::back_inserter(result), 
			[&op](char ch) { return is_ascii(ch) ? op(ch) : ch; });
		return result;
	}

	std::string lower(std::string_view str);
	std::string upper(std::string_view str);

}