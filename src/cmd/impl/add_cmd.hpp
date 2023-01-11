#include "../cmd.hpp"

namespace lf {

    class add_cmd: public cmd {
    public:
        add_cmd();
        int run(const opt_map& opts) const override;
    };

}