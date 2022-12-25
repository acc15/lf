#include "cmd.hpp"

namespace lf {

    class add_cmd: public cmd {
    public:
        
        static const char* names[];
        static const cmd_desc desc;

        add_cmd();
        int run(const std::span<const char*>& args) const override;
    };

}