#pragma once

#include <span>
#include <string>
#include <string_view>
#include <istream>
#include <vector>
#include <unordered_map>

#include "config_sync.hpp"
#include "../util/str_hash.hpp"
#include "../io/errors.hpp"

namespace lf {

class config {
public:
	void load(errors& err, std::istream& in);
	
	const std::vector<config_sync>& syncs() const;
	const config_sync* find(const std::string& name) const;

private:
	static const char* TOKENS[6];
	typedef const std::string_view token_array[std::size(TOKENS)];

	bool validate_tokens(errors& err, token_array tokens);
	bool parse_sync(errors& err, token_array tokens, config_sync& sync);
	bool parse_mirror(errors& err, token_array tokens, size_t token_offset, config_mirror& mirror);

	std::vector<config_sync> _syncs;
	std::unordered_map<std::string, std::size_t, str_hash, str_hash::eq> _names;

};

}