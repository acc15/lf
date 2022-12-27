#pragma once

#include <cstring>

#include <string_view>
#include <filesystem>
#include <fstream>

#include "../fs/path.hpp"

#include "log.hpp"
#include "with_format.hpp"

namespace lf {

	template <auto Format, typename T>
	bool load_file(const std::filesystem::path& path, std::string_view name, T& ref) {
		log.info() && log() << "loading " << name << " from " << path << "..." << std::endl;
        std::ifstream file(path);
        if (!file) {
            log.error() && log() << "unable to open file " << path << " for reading: " << strerror(errno) << std::endl;
            return false;
        }

        file >> with_ref_format<Format>(ref);
        if (file.fail() || file.bad()) {
            log.debug() && log() << "load file " << path << " failed with "
				<< (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
				<< std::endl;
			return false;
        }

        if (!file.eof()) {
            log.trace() && log() << "file " << path << "wasn't fully read (no eofbit)" << std::endl;
        }
        return true;
	}

	template <auto Format, typename T>
	bool save_file(const std::filesystem::path& path, std::string_view name, const T& ref) {
		log.info() && log() << "saving " << name << " to " << path << "..." << std::endl;
        
        if (!create_parent_dirs(path)) {
            log.error() && log() << "unable to create parent directories for " << path << std::endl;
            return false;
        }

        std::ofstream file(path);
        if (!file) {
            log.error() && log() << "unable to open file at " << path << " for writing: " << strerror(errno) << std::endl;
            return false;
        }

        file << with_ref_format<Format>(ref);
        if (file.fail() || file.bad()) {
            log.debug() && log() << "save file " << path << " failed with "
                << (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
				<< std::endl;
            return false;
        }
        return true;
	}

}