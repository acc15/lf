#include "cmd/cmd.hpp"

namespace lf {

    class add_cmd: public cmd {
    public:
        explicit add_cmd();
        bool run(const opt_map& params) const override;
    };

}