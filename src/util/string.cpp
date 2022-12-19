#include "string.hpp"

namespace leafsync {

	std::string_view rtrim(std::string_view str) {
		return str.substr(0, str.size() - count_first_spaces(str.rbegin(), str.rend()));
	}

	std::string_view ltrim(std::string_view str) {
		return str.substr(count_first_spaces(str.begin(), str.end()));
	}

	std::string_view trim(std::string_view str) {
		return ltrim(rtrim(str));
	}

	std::string lower(std::string_view str) {
		return transform_ansi(str, static_cast<int (*)(int)>(&std::tolower));
	}

	std::string upper(std::string_view str) {
		return transform_ansi(str, static_cast<int (*)(int)>(&std::toupper));
	}

}