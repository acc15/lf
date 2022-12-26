#include <vector>
#include <string>
#include <sstream>
#include "io/log.hpp"

namespace lf {

    struct log_tester {
        log_tester(log::level level);
        ~log_tester();
        
        std::string str() const;
        std::vector<std::string> lines();

        std::stringstream out;
        log::level original_min_level;
    };

}