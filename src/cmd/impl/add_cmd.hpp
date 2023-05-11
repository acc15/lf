#include "cmd/cmd.hpp"

namespace lf {

    class add_cmd: public cmd {
        static bool add_paths(cmd_context& ctx, sync_mode mode, const char* opt_name);
        static bool add_path(cmd_context& ctx, sync_mode mode, std::string_view path);
    
    public:
        explicit add_cmd();
        bool run(cmd_context& ctx) const override;

    };

}