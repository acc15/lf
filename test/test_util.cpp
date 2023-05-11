#include <catch2/interfaces/catch_interfaces_capture.hpp>
#include "test_util.hpp"
#include "fs/util.hpp"

#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace lf {

    const fs::path test_dir_path = fs::path(__FILE__).parent_path();
    
#ifdef _WIN32
    const fs::path test_root_path = "C:\\";
#else
    const fs::path test_root_path = "/";
#endif

    const fs::path test_sample_path = fs::path("a") / "b" / "c";
    const fs::path test_sample_path2 = fs::path("x") / "y" / "z";

    cd_changer::cd_changer(const std::filesystem::path& p): restore_path(fs::current_path()) {
        fs::current_path(p);
    }

    cd_changer::~cd_changer() {
        fs::current_path(restore_path);
    }

    fs::path create_temp_test_dir(const fs::path& suffix) {
        fs::path p = fs::temp_directory_path() / "lf_test" / Catch::getResultCapture().getCurrentTestName();
        if (!suffix.empty()) {
            p /= suffix;
        }

        fs::create_directories(p);
        for (auto child: fs::directory_iterator(p)) {
            fs::remove_all(child);
        }
        return p;
    }

    std::string test_root_str(std::string_view p) {
        return (test_root_path / p).string();
    }

    void write_text(const std::filesystem::path& path, const std::string& text) {
        fs::create_directories(path.parent_path());
        std::ofstream s(path);
        s << text;
    }

    std::string read_text(const std::filesystem::path& path) {
        std::ifstream s(path);
        std::stringstream buf;
        s >> buf.rdbuf();
        return buf.str();
    }

    config make_test_config(const fs::path& suffix) {
        const auto test_dir = create_temp_test_dir(suffix);
        const auto local = test_dir / "local";
        const auto remote = test_dir / "remote";
        fs::create_directories(local);
        fs::create_directories(remote);
        return config {
            .local = local,
            .remote = remote,
            .state = local / "lf.state",
            .index = remote / "lf.index"
        };
    }

    cmd_context make_test_cmd_context(
        const config& cfg,
        const opt_map::map_type& opts,         
        const lf::index& index,
        const lf::state& state
    ) {
        return cmd_context {
            opt_map(opts), 
            cfg, 
            lazy_tree<lf::index>(cfg.index, index),
            lazy_tree<lf::state>(cfg.state, state),
        };
    }

}
