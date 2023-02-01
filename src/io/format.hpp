#pragma once

#include <concepts>

namespace lf {

	template <typename T>
	concept format_type = requires {
		{ T::binary } -> std::convertible_to<bool>;
	};

	template <bool Binary>
	struct format {
		static const bool binary = Binary;
	};

	template <format_type Format, typename T>
	struct with_format {
		using value_type = T;
		using reference_type = value_type&;
		using format_type = Format;

		reference_type value;
	};

	template <format_type Format, typename T>
	constexpr auto write_as(const T& ref) {
		return with_format<Format, const T> { ref };
	}

	template <format_type Format, typename T>
	constexpr auto read_as(T& ref) {
		return with_format<Format, T> { ref };
	}

}