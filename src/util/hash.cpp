#include "hash.hpp"

namespace leafsync {

    size_t str_hash::operator()(std::string_view txt) const { 
        return impl{}(txt); 
    }

    size_t str_hash::operator()(const std::string& txt) const { 
        return impl{}(txt); 
    }

    size_t str_hash::operator()(const char* txt) const { 
        return impl{}(txt); 
    }

}