#include <catch2/catch_test_macros.hpp>

#include "io/time.hpp"

using namespace std::chrono;

std::string format(std::chrono::milliseconds::rep t) {
    std::ostringstream s;
    s << lf::format_date_time(system_clock::time_point(milliseconds(t)), true);
    return s.str();
}

TEST_CASE("datetime format_date_time", "[datetime]") {

    REQUIRE( format(1673953910406L) == "2023-01-17T11:11:50.406" );
    REQUIRE( format(1673953910006L) == "2023-01-17T11:11:50.006" );
    REQUIRE( format(1673953910031L) == "2023-01-17T11:11:50.031" );

}