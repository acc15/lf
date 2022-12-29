#pragma once

#include <cstring>

#include <string_view>
#include <filesystem>
#include <fstream>
#include <concepts>
#include <exception>

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
	void load_file(const std::filesystem::path& path, T& result) {
        log.debug() && log() << "loading " << T::name << " file from " << path << "..." << std::endl;
        std::ifstream file(path);
        
        if (!file) {
            throw std::runtime_error((std::stringstream() 
                << "unable to open " << T::name << " file " << path << " for reading: " << strerror(errno)
            ).str());
        }

        file >> with_ref_format<T::format>(result);
        if (file.fail() || file.bad()) {
            throw std::runtime_error((std::stringstream() 
                << "unable to read " << T::name << " file from " << path << ", failed with"
                << (file.fail() ? " failbit" : "") 
                << (file.bad() ? " badbit" : "")
            ).str());
        }

        if (!file.eof()) {
            log.trace() && log() << T::name << " file " << path << "wasn't fully read (no eofbit)" << std::endl;
        }

        log.debug() && log() << T::name << " file has been successfully loaded from " << path << std::endl;
	}

	template <serializable T>
	T load_file(const std::filesystem::path& path) {
        T result;
        load_file(path, result);
        return result;
	}

	template <serializable T>
	void save_file(const std::filesystem::path& path, const T& ref) {
		log.debug() && log() << "saving " << T::name << " to " << path << "..." << std::endl;
        create_parent_dirs(path);

        std::ofstream file(path);
        if (!file) {
            throw std::runtime_error((std::stringstream() 
                << "unable to open file at " << path << " for writing: " << strerror(errno)
            ).str());
        }

        file << with_ref_format<T::format>(ref);
        if (file.fail() || file.bad()) {
            throw std::runtime_error((std::stringstream() 
                << "save file " << path << " failed with "
                << (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
            ).str());
        }

        log.debug() && log() << T::name << " has been successfully saved to " << path << std::endl;
	}

}