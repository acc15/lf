#include "config_mode.hpp"

namespace leafsync {

	config_mode parse_config_mode(const char ch) {
		switch (ch) {
		case 'r':
		case 'R':
			return config_mode::READ;
		case 'w':
		case 'W':
			return config_mode::WRITE;
		default:
			return config_mode::UNKNOWN;
		}
	}

}
