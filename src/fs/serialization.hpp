#pragma once

#include "io/adv_fstream.hpp"
#include <string_view>
#include <filesystem>
#include <concepts>
#include <exception>

#include "fs/util.hpp"
#include "log/log.hpp"
#include "io/openmode.hpp"
#include "io/format_stream.hpp"
#include "io/format.hpp"

namespace lf {

    template <typename T>
    concept serializable_type = requires {
        {T::name} -> std::convertible_to<std::string_view>;
    };

    constexpr std::ios_base::openmode OPEN_READ_LOCK = std::ios_base::in | std::ios_base::out;
    constexpr std::ios_base::openmode OPEN_READ_WRITE_LOCK = std::ios_base::in 
        | std::ios_base::out 
        | std::ios_base::app 
        | std::ios_base::ate;

    template <format_type Format, serializable_type Data, typename Stream>
    bool open_and_lock(
        const std::filesystem::path& path, 
        Stream& file, 
        std::ios_base::openmode mode = default_openmode<Stream>::default_mode
    ) {
        
        std::ios_base::openmode target_mode = mode | Format::openmode;

        log.debug() && log() 
            << "opening " << path << " with mode " << write_as<openmode_format>(target_mode) 
            << log::end;

        file.open(path, target_mode);
        if (!file) {
            throw_fs_error_if_exists(
                format_stream() << "unable to open " 
                    << Data::name << " file with flags " << write_as<openmode_format>(mode), 
                path);
            return false;
        }
        
        bool exclusive = mode & std::ios_base::out;
        log.debug() && log() << "locking " << path << " with exclusive " << exclusive << log::end;

        file.lock(exclusive);
        if (!file) {
            throw std::filesystem::filesystem_error(
                format_stream() << "unable to lock " 
                    << Data::name << " file with flags " << write_as<openmode_format>(mode), 
                path, 
                std::error_code(EAGAIN, std::iostream_category()));
        }

        log.debug() && log() << "file " << path 
            << " has been successfully opened with mode " << write_as<openmode_format>(target_mode)
            << " and locked exclusive " << exclusive << log::end;

        return true;
    }

	template <format_type Format, serializable_type Data>
	bool load_file(const std::filesystem::path& path, Data& result) {
        adv_ifstream file;
        if (!open_and_lock<Format, Data, adv_ifstream>(path, file, std::ios_base::in)) {
            return false;
        }
        load_file<Format, Data>(path, file, result);
        return true;
	}

    template <format_type Format, serializable_type Data>
    void load_file(const std::filesystem::path& path, std::istream& file, Data& ref) {
        log.debug() && log() << "loading " << Data::name << " file from " << path << "..." << log::end;
        
        file >> read_as<Format>(ref);
        if (file.fail() || file.bad()) {
            throw std::runtime_error(format_stream() 
                << "unable to read " << Data::name << " file from " << path << ", failed with"
                << (file.fail() ? " failbit" : "") 
                << (file.bad() ? " badbit" : "")
            );
        }

        if (!file.eof()) {
            log.trace() && log() << Data::name << " file " << path << "wasn't fully read (no eofbit)" << log::end;
        }

        log.debug() && log() << Data::name << " file has been successfully loaded from " << path << log::end;
    }

	template <format_type Format, serializable_type Data>
	Data load_file(const std::filesystem::path& path) {
        Data result;
        load_file<Format, Data>(path, result);
        return result;
	}

    template <format_type Format, serializable_type Data>
    void save_file(const std::filesystem::path& path, std::ostream& file, const Data& ref) {
		log.debug() && log() << "saving " << Data::name << " to " << path << "..." << log::end;

        file << write_as<Format>(ref);
        if (file.fail() || file.bad()) {
            throw std::runtime_error(format_stream() 
                << "save file " << path << " failed with "
                << (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
            );
        }

        log.debug() && log() << Data::name << " has been successfully saved to " << path << log::end;
    }

    template <format_type Format, serializable_type Data>
    void save_file(const std::filesystem::path& path, const Data& ref) {
        adv_ofstream file;
        if (!open_and_lock<Format, Data, adv_ofstream>(path, file, std::ios_base::out)) {
            return;
        }
        save_file<Format, Data>(path, file, ref);
    }

}