#include "string.hpp"

namespace leafsync {

	std::string_view rtrim(const std::string_view& str) {
		return str.substr(0, str.size() - count_first_spaces(str.rbegin(), str.rend()));
	}

	std::string_view ltrim(const std::string_view& str) {
		return str.substr(count_first_spaces(str.begin(), str.end()));
	}

	std::string_view trim(const std::string_view& str) {
		return ltrim(rtrim(str));
	}

}