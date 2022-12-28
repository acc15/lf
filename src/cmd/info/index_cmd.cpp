#include "cmd/info/index_cmd.hpp"

namespace lf {

    index_cmd::index_cmd(): cmd({"i", "index"}, nullptr, "prints currrently indexed files and their sync modes") {
    }
    
    int index_cmd::run(const std::span<const char*>& args) const {

        return 0;
    }

}