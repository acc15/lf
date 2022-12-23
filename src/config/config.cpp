#include <cassert>
#include <algorithm>
#include <string>
#include <string_view>

#include <fmt/core.h>

#include "config.hpp"
#include "../util/string.hpp"
#include "../util/tokenizer.hpp"
namespace leafsync {

	const char* config::TOKENS[] = { "sync name", "index path", "left mode", "left path", "right mode", "right path" };

	const std::vector<config_sync>& config::syncs() const {
		return _syncs;
	}

	const config_sync* config::find(const std::string& name) const {
		const auto it = _names.find(name);
		return it == _names.end() ? nullptr : &_syncs[it->second];
	}

	void config::load(errors& err, std::istream& in) {

		_names.clear();
		_syncs.clear();

		std::string line;

		std::string_view tokens[std::size(TOKENS)];
		const std::string_view separators[std::size(TOKENS)] = { ":", "//", ":", "//", ":", "" };

		tokenizer tok;

		err.loc.line = 0;
		while (std::getline(in, line)) {
			++err.loc.line;

			std::string_view trimmed = trim(line);
			if (trimmed.empty() || trimmed.starts_with("#")) {
				continue;
			}

			tok.chunk = trimmed;

			if (!tok.parse(tokens, separators)) {
				err(fmt::format("illegal configuration line format: {}", trimmed));
				continue;
			}

			if (!validate_tokens(err, tokens)) {
				continue;
			}

			if (_names.contains(tokens[0])) {
				err(fmt::format("duplicate sync: {}", tokens[0]));
				continue;
			}

			size_t index = _syncs.size();
			
			config_sync& sync = _syncs.emplace_back();
			if (!parse_sync(err, tokens, sync)) {
				_syncs.pop_back();
				continue;
			}

			const auto ins = _names.emplace(tokens[0], index);
			assert(ins.second);

			sync.name = ins.first->first;

		}

	}

	bool config::validate_tokens(errors& err, token_array tokens) {
		bool valid = true;
		for (size_t i = 0; i < std::size(TOKENS); i++) {
			if (tokens[i].empty()) {
				err(fmt::format("{} can't be empty", TOKENS[i]));
				valid = false;
			}
		}
		return valid;
	}

	bool config::parse_sync(errors& err, token_array tokens, config_sync& sync) {
		sync.index = tokens[1];
		bool pl = parse_mirror(err, tokens, 2, sync.left);
		bool pr = parse_mirror(err, tokens, 4, sync.right);
		return pl && pr;
	}

	bool config::parse_mirror(errors& err, token_array tokens, size_t token_offset, config_mirror& mirror) {
		if (!parse_config_mode(tokens[token_offset], mirror.mode)) {
			err(fmt::format("illegal {}: {}", TOKENS[token_offset], tokens[token_offset]));
			return false;
		}
		
		mirror.path = tokens[token_offset + 1];
		if (!mirror.path.is_absolute()) {
			err(fmt::format("{} must be absolute: {}", TOKENS[token_offset + 1], tokens[token_offset + 1]));
			return false;
		}
		return true;
	}

}