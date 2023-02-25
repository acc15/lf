#include <catch2/catch_test_macros.hpp>

#include <ostream>
#include <filesystem>
#include "fs/adv_fstream.hpp"

template <typename T>
struct test_data {
    T data;
};

template <typename Serializer, typename Stream = lf::adv_fstream>
class file_serializer;

template <typename Serializer, typename DataType, bool Binary>
struct base_serializer {
    
    using serializer_type = Serializer;
    using data_type = DataType;
    static constexpr bool binary = Binary;

    template<typename Stream = lf::adv_fstream>
    file_serializer<Serializer, Stream> file(const std::filesystem::path& path) {
        return file_serializer<Serializer, Stream>(path);
    }

};

template <typename T>
struct test_serializer: base_serializer<test_serializer<T>, T, true> {
};

template <typename T>
struct test_serializer<test_data<T>>: base_serializer<test_serializer<test_data<T>>, test_data<T>, true> {
    void write(std::ostream& s, const test_data<T>& data) {
        test_serializer<T>::write(s, data.data);
    }
};

template <>
struct test_serializer<bool>: base_serializer<test_serializer<bool>, bool, true> {
    void write(std::ostream& s, const bool& data) {
    }
};

template <typename Serializer, typename Stream>
class file_serializer {
public:

    using serializer_type = Serializer;
    using fstream = Stream;
    using data_type = typename Serializer::data_type;

    file_serializer(const std::filesystem::path& p): _path(p), _stream() {
    }

    file_serializer(file_serializer&& m): _path(m._path), _stream(std::move(m._stream)) {
    }

    file_serializer& open(std::ios_base::openmode mode) {
        _stream.open(_path, mode);
        return *this;
    }

    file_serializer& lock(bool exclusive, bool wait = true) {
        _stream.lock(exclusive, wait);
        return *this;
    }

    file_serializer& read(data_type& data) {
        serializer_type::read(_stream, data);
        return *this;
    }

    file_serializer& write(const data_type& data) {
        serializer_type::write(_stream, data);
        return *this;
    }
    
    file_serializer& close() {
        _stream.close();
        return *this;
    }

    const std::filesystem::path& path() const {
        return _path;
    }

    fstream& stream() {
        return _stream;
    }

private:
    const std::filesystem::path& _path;
    fstream _stream;

};

TEST_CASE("test", "[new_format]") {
    
    std::stringstream s;

    test_data<bool> d = { false };

    using serializer = test_serializer<test_data<bool>>;
    serializer ser;
    ser.file<lf::adv_ifstream>("abc").open(std::ios_base::in | std::ios_base::out | std::ios_base::app | std::ios_base::ate).lock(true);

    // ::write(s, d);

}