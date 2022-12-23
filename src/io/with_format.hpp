#pragma once

#include <string_view>

#include "errors.hpp"

namespace leafsync {

	enum class format: size_t {
		BINARY
	};

	template <auto Format, typename T>
	struct with_format {
		T value;

		template <typename V>
		with_format<Format, V&> with_ref(V& new_ref) const {
			return with_format<Format, V&> { new_ref };
		}

		template <typename V>
		with_format<Format, V> with_value(V new_value) const {
			return with_format<Format, V> { new_value };
		}
	};

	template <auto Format, typename T>
	struct with_format_and_errors: with_format<Format, T> {
		errors& err;

		template <typename V>
		with_format_and_errors<Format, V&> with_ref(V& new_ref) const {
			return with_format_and_errors<Format, V&> { new_ref, err };
		}
	};

	template <auto Format, typename T>
	with_format<Format, T&> with_ref_format(T& ref) {
		return with_format<Format, T&> { ref };
	}

	template <auto Format, typename T>
	with_format_and_errors<Format, T&> with_ref_format(T& ref, errors& e) {
		return with_format_and_errors<Format, T&> { ref, e };
	}

}