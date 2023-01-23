#pragma once

#include <string_view>
#include <string>

namespace lf {

	bool is_ascii(char c);
	bool is_whitespace(char ch);
	char lower(char ch);
	char upper(char ch);

	std::string_view rtrim(std::string_view str);
	std::string_view ltrim(std::string_view str);
	std::string_view trim(std::string_view str);

	std::string lower(std::string_view str);
	std::string upper(std::string_view str);

	void lower(std::string& str);
	void upper(std::string& str);

	std::string lpad(std::string_view str, std::string::size_type len, char pad);

	bool ci_char_eq(char a, char b);
	bool ci_str_eq(std::string_view a, std::string_view b);


}