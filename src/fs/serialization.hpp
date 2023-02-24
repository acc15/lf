#pragma once

#include "fs/adv_fstream.hpp"
#include <string_view>
#include <filesystem>
#include <concepts>
#include <exception>

#include "fs/util.hpp"
#include "log/log.hpp"
#include "io/ios_flags.hpp"
#include "io/format_stream.hpp"
#include "io/format.hpp"

namespace lf {

    template <typename T>
    concept serializable_type = format_type<typename T::format> && requires {
        {T::name} -> std::convertible_to<std::string_view>;
    };

    template <serializable_type T>
    constexpr std::ios_base::openmode get_serializable_openmode() {
        return static_cast<std::ios_base::openmode>(T::format::binary ? std::ios_base::binary : 0);
    }

    template <typename Stream>
    bool open_and_lock(
        const std::filesystem::path& path, 
        Stream& file, 
        const char* name, 
        std::ios_base::openmode mode
    ) {
        file.open(path, mode);
        if (!file) {
            throw_fs_error(
                format_stream() << "unable to open " << name << " file with flags " << write_as<ios_flags_format>(mode), 
                path);
            return false;
        }
        
        file.lock(mode & std::ios_base::out);
        if (!file) {
            throw std::filesystem::filesystem_error(
                format_stream() << "unable to lock " << name << " file with flags " << write_as<ios_flags_format>(mode), 
                path, 
                std::error_code(EAGAIN, std::iostream_category()));
        }

        return true;
    }

	template <serializable_type T>
	bool load_file(const std::filesystem::path& path, T& result) {
        const std::ios_base::openmode flags = std::ios_base::in | get_serializable_openmode<T>();
 
        adv_ifstream file;
        if (!open_and_lock(path, file, T::name, flags)) {
            return false;
        }
        load_file(path, file, result);
        return true;
	}

    template <serializable_type T>
    void load_file(const std::filesystem::path& path, std::istream& file, T& ref) {
        log.debug() && log() << "loading " << T::name << " file from " << path << "..." << log::end;
        
        file >> read_as<typename T::format>(ref);
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

	template <serializable_type T>
	T load_file(const std::filesystem::path& path) {
        T result;
        load_file(path, result);
        return result;
	}

    template <serializable_type T>
    void save_file(const std::filesystem::path& path, std::ostream& file, const T& ref) {
		log.debug() && log() << "saving " << T::name << " to " << path << "..." << log::end;

        file << write_as<typename T::format>(ref);
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