#pragma once

#include <string_view>

#include "errors.hpp"

namespace leafsync {

	enum class format: size_t {
		BINARY
	};

	template <auto Format, typename T>
	struct with_format {
		T& value;

		template <typename T>
		with_format<Format, T> with(T& new_ref) const {
			return with_format<Format, T> { new_ref };
		}
	};

	template <auto Format, typename T>
	struct with_format_and_errors: with_format<Format, T> {
		errors& err;

		template <typename T>
		with_format_and_errors<Format, T> with(T& new_ref) const {
			return with_format_and_errors<Format, T> { new_ref, err };
		}
	};

	template <auto Format, typename T>
	with_format<Format, T> formatted_as(T& ref) {
		return with_format<Format, T> { ref };
	}

	template <auto Format, typename T>
	with_format_and_errors<Format, T> formatted_as(T& ref, errors& e) {
		return with_format_and_errors<Format, T> { ref, e };
	}

}