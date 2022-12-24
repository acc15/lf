#pragma once

#include <istream>
#include <filesystem>

#include "config_mode.hpp"

namespace lf {

struct config_mirror {
	config_mode mode;
	std::filesystem::path path;
};

}
