#include "../cmd.hpp"

namespace lf {

    class add_cmd: public cmd {
    public:
        add_cmd();
        int run(const std::span<const char*>& args) const override;
    };

}