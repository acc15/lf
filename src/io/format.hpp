#pragma once

namespace lf {

	enum class format {
		BINARY,
		TEXT
	};

	template <auto Format, typename T>
	struct with_format {
		T value;
	};

	template <auto Format, typename T>
	with_format<Format, const T&> with_cref_format(const T& ref) {
		return with_format<Format, const T&> { ref };
	}

	template <auto Format, typename T>
	with_format<Format, T&> with_ref_format(T& ref) {
		return with_format<Format, T&> { ref };
	}

}