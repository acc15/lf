#pragma once

#include <string_view>

namespace leafsync {

	enum class config_mode {
		READ,
		WRITE
	};

	bool parse_config_mode(std::string_view str, config_mode& result);

}
