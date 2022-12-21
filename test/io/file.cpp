#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <thread>

#include <fmt/core.h>
#include <fmt/chrono.h>

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

struct file_exception {
    int code;

    const char* what() const {
        return strerror(code);
    }
};

class file {
public:

    ~file() noexcept {
        close();
    }

    bool is_open() noexcept {
        return _handle != nullptr;
    }

    FILE* handle() const {
        return _handle;
    }

    void close() noexcept {
        if (_handle != nullptr) {
            fclose(_handle);
            _handle = nullptr;
        }
    }

    void open(const char* path, const char* mode) noexcept(false) {
        _handle = _handle == nullptr ? fopen(path, mode) : freopen(path, mode, _handle);
        if (_handle == nullptr) {
            throw file_exception { .code = errno };
        }
    }

    size_t read(void* buf, size_t size) noexcept(false) {
        return fread(buf, 1, size, non_null_handle());
    }

    size_t write(const void* data, size_t size) noexcept(false) {
        return fwrite(data, 1, size, non_null_handle());
    }

    bool eof() const noexcept(false) {
        return feof(non_null_handle());
    }

    void flush() noexcept(false) {
        throw_if_error( fflush(non_null_handle()) );
    }

    int fd() const noexcept(false) {
        return throw_if_error( fileno(non_null_handle()) );
    }

private:

    int throw_if_error(int result) const {
        if (result == -1) {
            throw file_exception { .code = errno };
        }
        return result;
    }

    FILE* non_null_handle() const {
        if (_handle == nullptr) {
            throw file_exception { .code = EBADF };
        }
        return _handle;
    }

    FILE* _handle = nullptr;
};

TEST_CASE("fcntl", "[.][file]") {

    file f;

    const char* path = "/tmp/рус.txt";
    f.open(path, "wt");
    int fd = f.fd();

    fmt::print("file {}, fd: {}\n", path, fd);
    flock l = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
        .l_pid = 0
    };

    fmt::print("trying to lock file {}\n", path);
    if (fcntl(fd, F_SETLKW, &l) == -1) {
        fmt::print("unable to set write lock: {}", strerror(errno));
        return;
    }    

    const auto delay = std::chrono::seconds(10);
    fmt::print("file {} locked. sleeping {}\n", path, delay);

    std::this_thread::sleep_for(delay);

    const char str[] = "first line\n";
    f.write(str, std::size(str));
    f.flush();

    std::this_thread::sleep_for(delay);

    const char str2[] = "second line\n";
    f.write(str2, std::size(str2));
    f.flush();

    std::this_thread::sleep_for(delay);

}