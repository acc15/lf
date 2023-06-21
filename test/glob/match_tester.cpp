#include "match_tester.hpp"

namespace lf {

    match_tester::match_tester(const test_glob::string& string): stream(string), buf(stream.rdbuf()) {
    }

    std::size_t match_tester::pos() const {
        return static_cast<std::size_t>(buf->pubseekoff(0, std::ios_base::cur, std::ios_base::in));
    }

    std::size_t match_tester::size() const {
        return stream.view().size();
    }

}