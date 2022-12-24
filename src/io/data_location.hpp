#pragma once

#include <string>

namespace lf {

	struct data_location {
		std::string source = "";
		size_t line = 0;
		size_t column = 0;
	};

}