#pragma once

#include <span>
#include <string_view>
#include <istream>
#include <vector>
#include <unordered_map>

#include "config_sync.hpp"
#include "../err/error_handler.hpp"

namespace leafsync {

class config {
public:
	void load(error_handler& err, std::istream& in);
	
	const std::vector<config_sync>& syncs() const;
	const config_sync* find(const std::string& name) const;

private:
	static const char* TOKENS[6];
	typedef const std::string_view token_array[std::size(TOKENS)];

	bool validate_tokens(error_handler& err, token_array tokens);
	bool parse_sync(error_handler& err, token_array tokens, config_sync& sync);
	bool parse_mirror(error_handler& err, token_array tokens, size_t token_offset, config_mirror& mirror);

	std::vector<config_sync> _syncs;
	std::unordered_map<std::string, std::size_t> _names;

};

}