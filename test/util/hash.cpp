#include <catch2/catch_test_macros.hpp>

#include <unordered_map>
#include <util/hash.hpp>

using namespace std::literals;
using namespace lf;

void init_hash(size_t v[3], const char* variant, const str_hash& h) {
    v[0] = h(variant);
    v[1] = h(std::string_view(variant));
    v[2] = h(std::string(variant));
}

TEST_CASE("str_hash", "[hash]") {
    str_hash h;

    const char* variants[] = { "abc", "xyz" };

    for (size_t i = 0; i < std::size(variants); i++) {
        
        size_t iv[3];
        init_hash(iv, variants[i], h);
        
        for (size_t j = 0; j < std::size(variants); j++) {
            
            size_t jv[3];
            init_hash(jv, variants[j], h);

            for (size_t k = 0; k < std::size(iv); k++) {
                for (size_t l = 0; l < std::size(jv); l++) {
                    if (i == j) {
                        REQUIRE( iv[k] == jv[l] );
                    } else {
                        REQUIRE( iv[k] != jv[l] );
                    }
                }
            }
        }
    }

}

TEST_CASE("heterogenous_lookup", "[hash]") {
    std::unordered_map<std::string, int, str_hash, str_hash::eq> map;
    map["abc"] = 11;
    
    REQUIRE( map.contains("abc"sv) );
    REQUIRE_FALSE( map.contains("xyz"sv) );
}