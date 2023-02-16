#pragma once

#include <iostream>
#include <stdio.h>

#ifdef _WIN32
#   define private protected // Very bad ugly hack but better than reimplement basic_filebuf
#   include <fstream>
#   undef private // End of very bad and ugly hack
#   include <Windows.h>
#else
#   include <fstream>
#endif 

namespace lf {

    template <typename Elem, typename Traits>
    class basic_adv_filebuf: public std::basic_filebuf<Elem, Traits> {

#if defined (__GLIBCXX__)
    public:
        FILE* file() const {
            return const_cast<basic_file&>(_M_file).file();
        }

        int fd() const {
            return const_cast<basic_file&>(_M_file).fd();
        }

#elif defined (_MSC_VER)
        using std::basic_filebuf<Elem, Traits>::_Myfile;
    public:
        FILE* file() const {
            return _Myfile;
        }

        int fd() const {
            return _fileno(_Myfile);
        }

#ifdef _WIN32
        HANDLE handle() const {
            return _get_osfhandle(fd());
        }
#endif

#else
#   error "Unsupported platform!!!"
#endif

        basic_adv_filebuf() {}
        basic_adv_filebuf(basic_adv_filebuf&& m): std::basic_filebuf<Elem, Traits>(std::move(m)) {}
        ~basic_adv_filebuf() noexcept override {}

    };

    /**
    @brief fstream with additional features:
    - file locking (OS dependent: fcntl on POSIX or LockFile on Windows)
    - access to internal FILE* and file descriptor
    - ability to truncate file after creation
    **/
    /*template <typename Char, typename Traits, typename Parent, std::ios_base::openmode DefaultMode, std::ios_base::openmode AddMode = 0>
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

    };*/

}