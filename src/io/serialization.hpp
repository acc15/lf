#pragma once

#include <cstring>

#include <string_view>
#include <filesystem>
#include <fstream>
#include <concepts>
#include <typeinfo>

#include "../fs/path.hpp"

#include "log.hpp"
#include "format.hpp"

namespace lf {

    template <typename T>
    concept serialization_desc = requires {
        typename T::type;
        T::format;
        {T::name} -> std::convertible_to<std::string_view>;
    };

	template <serialization_desc Desc>
	bool load_file(const std::filesystem::path& path, typename Desc::type& ref) {
        log.debug() && log() << "loading " << Desc::name << " from " << path << "..." << std::endl;
        std::ifstream file(path);
        if (!file) {
            log.error() && log() << "unable to open file " << path << " for reading: " << strerror(errno) << std::endl;
            return false;
        }

        file >> with_ref_format<Desc::format>(ref);
        if (file.fail() || file.bad()) {
            // assumes that real error already printed
            log.error() && log() << "load file " << path << " failed with "
				<< (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
				<< std::endl;
			return false;
        }

        if (!file.eof()) {
            log.trace() && log() << "file " << path << "wasn't fully read (no eofbit)" << std::endl;
        }

        log.debug() && log() << Desc::name << " has been successfully loaded from " << path << std::endl;
        return true;
	}

	template <serialization_desc Desc>
	bool save_file(const std::filesystem::path& path, const typename Desc::type& ref) {
		log.debug() && log() << "saving " << Desc::name << " to " << path << "..." << std::endl;
        
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

        file << with_ref_format<Desc::format>(ref);
        if (file.fail() || file.bad()) {
            log.error() && log() << "save file " << path << " failed with "
                << (file.fail() ? " failbit" : "") 
				<< (file.bad() ? " badbit" : "") 
				<< std::endl;
            return false;
        }

        log.debug() && log() << Desc::name << " has been successfully saved to " << path << std::endl;
        return true;
	}

}