#pragma once

#include <cassert>
#include <iostream>
#include <stdio.h>

#ifdef _WIN32
#   define private protected // Very bad ugly hack but better than reimplement basic_filebuf
#   include <fstream>
#   undef private // End of very bad and ugly hack
#   include <io.h>
#else
#   include <fstream>
#   include <unistd.h>
#   include <fcntl.h>
#endif 

namespace lf {

    template <typename Elem, typename Traits>
    class basic_adv_filebuf: public std::basic_filebuf<Elem, Traits> {

        using filebuf = std::basic_filebuf<Elem, Traits>;

#if defined (__GLIBCXX__)

        bool _lock(short int type, bool wait) {
            flock l = { type, SEEK_SET, 0, 0, 0 };
            return fcntl(fd(), wait ? F_SETLKW : F_SETLK, &l) != -1;
        }

    public:
        FILE* file() const {
            return const_cast<typename filebuf::__file_type&>(filebuf::_M_file).file();
        }

        int fd() const {
            return filebuf::_M_file.is_open() 
                ? const_cast<typename filebuf::__file_type&>(filebuf::_M_file).fd() 
                : -1;
        }

        bool lock(bool exclusive, bool wait) {
            return _lock(exclusive ? F_WRLCK : F_RDLCK, wait);
        }

        bool unlock() {
            return _lock(F_UNLCK, false);
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

        HANDLE handle() const {
            return _get_osfhandle( fd() );
        }

        bool lock(bool exclusive, bool wait) {
            DWORD flags = (exclusive ? LOCKFILE_EXCLUSIVE_LOCK : 0) | (wait ? 0 : LOCKFILE_FAIL_IMMEDIATELY);
            OVERLAPPED ov = { 0 };
            return LockFileEx(handle(), flags, MAX_DWORD, MAX_DWORD, &ov);
        }

        bool unlock() {
            OVERLAPPED ov = { 0 };
            return UnlockFileEx(handle(), 0, MAX_DWORD, MAX_DWORD, &ov);
        }

#endif

        bool truncate() {
            filebuf::seekpos(0);
            int d = fd();
            return 
#if defined(_MSC_VER) 
            _chsize(d, 0)
#else
            ftruncate(d, 0)
#endif
                == 0;
        }

        basic_adv_filebuf() {}
        basic_adv_filebuf(basic_adv_filebuf&& m): filebuf(std::move(m)) {}
        ~basic_adv_filebuf() noexcept override {}

    };

    /**
    @brief fstream with additional features:
    - file locking (OS dependent: fcntl on POSIX or LockFile on Windows)
    - access to internal FILE* and file descriptor
    - ability to truncate file after creation
    **/
    template <
        typename Parent, 
        std::ios_base::openmode DefaultMode, 
        std::ios_base::openmode AddMode = static_cast<std::ios_base::openmode>(0)
    >
    class basic_adv_fstream: public Parent {
    public:

        using stream_type = Parent;
        using char_type = typename Parent::char_type;
        using traits_type = typename Parent::traits_type;
    
        basic_adv_fstream(): stream_type(&_buf) {
        }

        template <typename PathLike>
        basic_adv_fstream(const PathLike& path, std::ios_base::openmode mode = DefaultMode): stream_type(&_buf) {
            open(path, mode);
        }

        basic_adv_fstream(basic_adv_fstream&& m): stream_type(std::move(m)), _buf(std::move(m._buf)) {
            stream_type::set_rdbuf(&_buf);
        }

        ~basic_adv_fstream() override {}

        bool is_open() const {
            return _buf.is_open();
        }

        template <typename PathLike>
        void open(const PathLike& path, std::ios_base::openmode mode = DefaultMode) {
            std::ios_base::openmode target_mode = mode | AddMode;
            if (_buf.open(path, target_mode) != nullptr) {
                stream_type::clear();
            } else {
                stream_type::setstate(std::ios_base::failbit);
            }
        }

        void close() {
            _buf.close();
        }

        void truncate() {
            if (_buf.truncate()) {
                stream_type::clear();
            } else {
                stream_type::setstate(std::ios_base::failbit);
            }
        }

        bool lock(bool exclusive, bool wait = true) {
            return _buf.lock(exclusive, wait);
        }

        bool unlock() {
            return _buf.unlock();
        }

        FILE* file() const {
            return _buf.file();
        }

        int fd() const {
            return _buf.fd();
        }

    private:
        basic_adv_filebuf<char_type, traits_type> _buf;
    };

    using adv_ifstream = basic_adv_fstream<std::basic_istream<char>, std::ios_base::in>;
    using adv_ofstream = basic_adv_fstream<std::basic_ostream<char>, std::ios_base::out>;
    using adv_fstream = basic_adv_fstream<std::basic_iostream<char>, std::ios_base::in | std::ios_base::out>;

}