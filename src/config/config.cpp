#include <algorithm>
#include <string>
#include <string_view>
#include <locale>

#include <fmt/core.h>

#include "config.hpp"
#include "../util/string.hpp"

namespace leafsync {

void config::load(std::istream& in) {

	std::string line;
	while (std::getline(in, line)) {

		std::string_view trimmed = trim(line);
		if (trimmed.empty() || trimmed.starts_with("#")) {
			continue;
		}

		fmt::print("line: {}\n", trimmed);

	}

}

}