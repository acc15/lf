#pragma once

#include <istream>
#include <filesystem>

#include "config_mode.hpp"

namespace leafsync {

struct config_mirror {
	std::filesystem::path path;
	config_mode mode;
};

}
