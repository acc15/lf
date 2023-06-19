#include <catch2/catch_test_macros.hpp>

#include <glob/glob.hpp>
#include <encoding/utf8.hpp>

using namespace lf;

using g = glob<utf8_encoding>;

TEST_CASE("star_matcher", "[matcher]") {

    // g::matcher_ptr m = std::make_unique<g::star_matcher>();
    // m.is_repetitive()
}