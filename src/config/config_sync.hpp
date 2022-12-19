#pragma once

#include <string>

#include "config_mirror.hpp"

namespace leafsync {

	struct config_sync {
		std::string_view name;
		std::filesystem::path index;
		config_mirror left;
		config_mirror right;
	};

}