#include "util/string.hpp"

namespace lf {

	bool is_ascii(char c) {
		return c >= 0;
	}

	template <std::input_iterator I, std::sentinel_for<I> S>
	size_t count_first_spaces(const I first, const S last) {
		return std::distance(first, std::find_if_not(first, last, [](const auto& ch) { return is_ascii(ch) && std::isspace(ch); }));
	}

	template <std::output_iterator<char> Out, std::invocable<char> UnaryOp>
	void transform_ascii(std::string_view str, Out out, UnaryOp op) {
		std::transform(str.begin(), str.end(), out, [&op](char ch) { return is_ascii(ch) ? op(ch) : ch; });
	}

	template <std::invocable<char> UnaryOp>
	std::string transform_ascii(std::string_view str, UnaryOp op) {
		std::string result;
		transform_ascii(str, std::back_inserter(result), op);
		return result;
	}

	std::string_view rtrim(std::string_view str) {
		str.remove_suffix(count_first_spaces(str.rbegin(), str.rend()));
		return str;
	}

	std::string_view ltrim(std::string_view str) {
		str.remove_prefix(count_first_spaces(str.begin(), str.end()));
		return str;
	}

	std::string_view trim(std::string_view str) {
		return ltrim(rtrim(str));
	}

	std::string lower(std::string_view str) {
		return transform_ascii(str, static_cast<int (*)(int)>(&std::tolower));
	}

	std::string upper(std::string_view str) {
		return transform_ascii(str, static_cast<int (*)(int)>(&std::toupper));
	}

	void lower_inplace(std::string& str) {
		transform_ascii(str, str.begin(), static_cast<int (*)(int)>(&std::tolower));
	}

	void upper_inplace(std::string& str) {
		transform_ascii(str, str.begin(), static_cast<int (*)(int)>(&std::toupper));
	}

}