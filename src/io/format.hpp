#pragma once

#include <concepts>
#include <ios>

namespace lf {

	template <typename T>
	concept format_type = requires {
		{ T::openmode } -> std::convertible_to<std::ios_base::openmode>;
	};

	template <bool Binary>
	struct format {
		static constexpr std::ios_base::openmode openmode = Binary 
			? std::ios_base::binary 
			: static_cast<std::ios_base::openmode>(0);
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