#include <fstream>
#include <filesystem>
#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include "test_util.hpp"

#if defined(__unix__)
    #include <fcntl.h>
    #include <unistd.h>
#elif defined(_WIN32)
    #include <Windows.h>
    #include <io.h>
#endif

namespace fs = std::filesystem;

class fd_filebuf: public std::filebuf {
public:
#if defined(__unix__)
    int fd() {
        return file.fd();
    }
#elif defined(_WIN32)
    HANDLE handle() {
        FILE* f = nullptr;
        return reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(f)));
    }
#endif
};


#if defined(__unix__)

TEST_CASE("file lock", "[.][lock]") {

    const char* path = "/mnt/router/tmp/test_lock.txt"; 

    int fd = open(path, O_RDWR);
    REQUIRE( fd != -1 );
    std::cout << "path " << path << ", fd: " << fd << std::endl;

    flock l = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
        .l_pid = 0
    };

    int result = fcntl(fd, F_SETLK, &l);
    if (result == -1) {
        int e = errno;
        if (e == EACCES) {
            std::cout << "file already locked!!!" << std::endl;
        } else {
            std::cout << "error during locking, errno = " << e << std::endl;
        }
        close(fd);
        return;
    }

    std::cout << "locked exclusively!!!";
    std::cin.get();

    close(fd);

}
#elif defined(_WIN32)
TEST_CASE("file lock", "[.][lock]") {

    std::cout << "__cplusplus = " << __cplusplus << " _HAS_CXX17 = " << _HAS_CXX17 << std::endl;

    // fs::path p = "Z:\\tmp\\test_lock.txt";
    // std::filebuf fb;
    // fb.open()
    // std::ifstream s("Z:\\tmp\\test_lock.txt");
     

}
#endif