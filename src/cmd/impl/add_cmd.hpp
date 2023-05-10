#include "cmd/cmd.hpp"

namespace lf {

    class add_cmd: public cmd {
        static bool add_paths_to_index(cmd_context& ctx, const char* opt_name, sync_mode mode);
    
    public:
        explicit add_cmd();
        bool run(cmd_context& ctx) const override;

    };

}