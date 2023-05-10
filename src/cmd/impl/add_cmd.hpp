#include "cmd/cmd.hpp"

namespace lf {

    class add_cmd: public cmd {
    public:
        explicit add_cmd();
        bool run(cmd_context& ctx) const override;

        static void add_paths_to_index(const std::vector<std::string_view>& paths, sync_mode mode);
    };

}