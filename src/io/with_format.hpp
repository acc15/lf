#pragma once

#include <string_view>

#include "errors.hpp"

namespace lf {

	enum class format {
		BINARY,
		YAML
	};

	template <auto Format, typename T>
	struct with_format {
		T value;
	};

	template <auto Format, typename T>
	struct with_format_and_errors: with_format<Format, T> {
		errors& err;
	};

	template <auto Format, typename T>
	with_format<Format, T&> with_ref_format(T& ref) {
		return with_format<Format, T&> { ref };
	}

	template <auto Format, typename T>
	with_format_and_errors<Format, T&> with_ref_format(T& ref, errors& e) {
		return with_format_and_errors<Format, T&> { { ref }, e };
	}

}