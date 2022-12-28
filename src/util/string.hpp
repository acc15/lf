#pragma once

#include <algorithm>
#include <locale>
#include <string_view>
#include <concepts>

namespace lf {

	bool is_ascii(char c);

	std::string_view rtrim(std::string_view str);
	std::string_view ltrim(std::string_view str);
	std::string_view trim(std::string_view str);

	std::string lower(std::string_view str);
	std::string upper(std::string_view str);

	void lower_inplace(std::string& str);
	void upper_inplace(std::string& str);

}