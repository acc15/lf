#include <catch2/catch_test_macros.hpp>

#include "io/datetime.hpp"

TEST_CASE("format_date_time", "[datetime]") {

    std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(1673139050L);
    std::ostringstream s;
    s << lf::format_date_time { .tp = tp, .utc = true };

    std::string str = s.str();
    REQUIRE( str == "2023-01-08T00:50:50" );

}