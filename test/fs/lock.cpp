#include <fstream>
#include <filesystem>
#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include <fcntl.h>
#include <unistd.h>

#include "test_util.hpp"

namespace fs = std::filesystem;

TEST_CASE("file lock get file descriptor", "[.][lock]") {

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