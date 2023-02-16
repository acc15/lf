#pragma once

#include <iostream>
#include <fstream>
#include <stdio.h>

namespace lf {

    template <typename Elem, typename Traits>
    class basic_adv_filebuf: public std::basic_filebuf<Elem, Traits> {
    public:
        ~basic_adv_filebuf() noexcept override {}

#if defined (__GLIBCXX__)

        FILE* file() const {
            return const_cast<basic_file&>(_M_file).file();
        }

        int fd() const {
            return const_cast<basic_file&>(_M_file).fd();
        }

#elif defined (_MSC_VER)

        FILE* file() const {
            return _file;
        }

        int fd() const {
            return _fileno(_file);
        }

    private:
        FILE* _file;
#else
#   error "Unsupported platform!!!"
#endif
    };

    /**
    @brief fstream with additional features:
    - file locking (OS dependent: fcntl on POSIX or LockFile on Windows)
    - access to internal FILE* and file descriptor
    - ability to truncate file after creation
    **/
    template <typename Char, typename Traits, typename Parent, std::ios_base::openmode DefaultMode, std::ios_base::openmode AddMode = 0>
    class basic_adv_fstream: public Parent {
    public:
        basic_adv_fstream(const char* path, std::ios_base::openmode mode = DefaultMode) {

        }

        basic_adv_fstream(const wchar_t* path, std::ios_base::openmode mode = DefaultMode) {

        }
        
        template <typename PathLike>
        basic_adv_fstream(const PathLike& path, std::ios_base::openmode mode = DefaultMode) {
            
        }

        ~basic_adv_fstream() override {}

    private:

    };

}