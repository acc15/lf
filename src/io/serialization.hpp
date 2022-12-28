#pragma once

#include <cstring>

#include <string_view>
#include <filesystem>
#include <fstream>
#include <concepts>

#include "fs/path.hpp"
#include "io/log.hpp"
#include "io/format.hpp"

namespace lf {

    template <typename T>
    concept serializable = requires {
        T::format;
        {T::name} -> std::convertible_to<std::string_view>;
    };

	template <serializable T>
	bool load_file(const std::filesystem::path& path, T& ref, bool optional = false) {
        log.debug() && log() << "loading " << T::name << " from " << path << "..." << std::endl;
        std::ifstream file(path);
        if (!file) {
            log.with(optional ? log::DEBUG : log::ERROR) && 
                log() << "unable to open " << T::name << " file " << path << " for reading: " << strerror(errno) << std::endl;
            return false;
        }

        file >> with_ref_format<T::format>(ref);
        if (file.fail() || file.bad()) {
            log.error() && log() << "loading of " << T::name << " file " << path << " failed with "
				<< (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
				<< std::endl;
			return false;
        }

        if (!file.eof()) {
            log.trace() && log() << T::name << " file " << path << "wasn't fully read (no eofbit)" << std::endl;
        }

        log.debug() && log() << T::name << " has been successfully loaded from " << path << std::endl;
        return true;
	}

	template <serializable T>
	bool save_file(const std::filesystem::path& path, const T& ref) {
		log.debug() && log() << "saving " << T::name << " to " << path << "..." << std::endl;
        
        try {
            create_parent_dirs(path);
        } catch (const std::runtime_error& e) {
            log.error() && log() << "unable to create parent directories for " << e.what() << std::endl;
            return false;
        }

        std::ofstream file(path);
        if (!file) {
            log.error() && log() << "unable to open file at " << path << " for writing: " << strerror(errno) << std::endl;
            return false;
        }

        file << with_ref_format<T::format>(ref);
        if (file.fail() || file.bad()) {
            log.error() && log() << "save file " << path << " failed with "
                << (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
				<< std::endl;
            return false;
        }

        log.debug() && log() << T::name << " has been successfully saved to " << path << std::endl;
        return true;
	}

}