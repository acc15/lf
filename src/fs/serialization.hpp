#pragma once

#include <string_view>
#include <filesystem>
#include <fstream>
#include <concepts>
#include <exception>

#include "fs/util.hpp"
#include "fs/error.hpp"
#include "log/log.hpp"
#include "io/ios_flags.hpp"
#include "io/format_stream.hpp"
#include "io/format.hpp"

namespace lf {

    template <typename T>
    concept serializable = requires {
        T::format;
        {T::binary} -> std::convertible_to<bool>;
        {T::name} -> std::convertible_to<std::string_view>;
    };

    template <serializable T>
    constexpr std::ios_base::openmode get_serializable_openmode() {
        return static_cast<std::ios_base::openmode>(T::binary ? std::ios_base::binary : 0);
    }

	template <serializable T>
	void load_file(const std::filesystem::path& path, T& result) {
        const std::ios_base::openmode flags = std::ios_base::in | get_serializable_openmode<T>();
        log.debug() && log() << "loading " << T::name << " file from " << path 
            << " with flags " << with_cref_format<format::TEXT>(flags) << "..." << log::end;
        
        std::ifstream file(path, flags);
        if (!file) {
            throw_fs_error(format_stream() << "unable to open " << T::name << " file for reading", path);
        }

        file >> with_ref_format<T::format>(result);
        if (file.fail() || file.bad()) {
            throw std::runtime_error(format_stream() 
                << "unable to read " << T::name << " file from " << path << ", failed with"
                << (file.fail() ? " failbit" : "") 
                << (file.bad() ? " badbit" : "")
            );
        }

        if (!file.eof()) {
            log.trace() && log() << T::name << " file " << path << "wasn't fully read (no eofbit)" << log::end;
        }

        log.debug() && log() << T::name << " file has been successfully loaded from " << path << log::end;
	}

	template <serializable T>
	T load_file(const std::filesystem::path& path) {
        T result;
        load_file(path, result);
        return result;
	}

	template <serializable T>
	void save_file(const std::filesystem::path& path, const T& ref) {
        const std::ios_base::openmode flags = std::ios_base::out | std::ios_base::trunc | get_serializable_openmode<T>(); 
		log.debug() && log() << "saving " << T::name << " to " << path 
            << " with flags " << with_cref_format<format::TEXT>(flags) << "..." << log::end;

        create_parent_dirs(path);

        std::ofstream file(path, flags);
        if (!file) {
            throw_fs_error(format_stream() << "unable to open " << T::name << " file for writing", path);
        }

        file << with_cref_format<T::format>(ref);
        if (file.fail() || file.bad()) {
            throw std::runtime_error(format_stream() 
                << "save file " << path << " failed with "
                << (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
            );
        }

        log.debug() && log() << T::name << " has been successfully saved to " << path << log::end;
	}

}