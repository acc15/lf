#pragma once

#include <istream>

namespace leafsync {

	enum class config_mode {
		UNKNOWN,
		READ,
		WRITE
	};

	config_mode parse_config_mode(const char ch);

}

// std::istream& operator>>(std::istream& in, leafsync::config_mode& mode);