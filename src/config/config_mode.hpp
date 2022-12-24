#pragma once

#include <string_view>

namespace lf {

	enum class config_mode {
		READ,
		WRITE
	};

	bool parse_config_mode(std::string_view str, config_mode& result);

}
