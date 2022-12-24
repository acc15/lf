#include <algorithm>
#include <string>
#include <unordered_map>

#include "config_mode.hpp"
#include "../util/string.hpp"

namespace lf {

	const std::unordered_map<std::string_view, config_mode> mode_names = {
		{"r", config_mode::READ},
		{"read", config_mode::READ},
		{"w", config_mode::WRITE},
		{"write", config_mode::WRITE}
	};

	bool parse_config_mode(std::string_view str, config_mode& result) {
		const auto it = mode_names.find(lower(str));
		if (it == mode_names.end()) {
			return false;
		}
		result = it->second;
		return true;
	}

}
