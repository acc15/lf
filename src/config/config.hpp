#pragma once

#include <istream>
#include <vector>
#include <unordered_map>

#include "config_sync.hpp"

namespace leafsync {

class config {
public:
	void load(std::istream& in);

private:
	std::vector<config_sync> _syncs;
	std::unordered_map<std::string, config_sync*> _names;

};

}