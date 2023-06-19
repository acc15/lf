#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <optional>

#include "config/config.hpp"
#include "cmd/cmd_context.hpp"

namespace lf {

    extern const std::filesystem::path test_dir_path;
    extern const std::filesystem::path test_root_path;
    extern const std::filesystem::path test_path1;
    extern const std::filesystem::path test_path2;
    extern const std::string test_path1_str;
    extern const std::string test_path2_str;

    class cd_changer {
        std::filesystem::path restore_path;
    public:
        cd_changer(const std::filesystem::path& p);
        ~cd_changer();
    };

    std::string test_root_str(std::string_view p); 
    std::filesystem::path create_temp_test_dir(const std::filesystem::path& suffix = std::filesystem::path());
    void write_text(const std::filesystem::path& path, const std::string& text);
    std::string read_text(const std::filesystem::path& path);

    config make_test_config(const std::filesystem::path& path = std::filesystem::path());

    cmd_context make_test_cmd_context(
        const config& cfg,
        const opt_map::map_type& opts, 
        const lf::index& index = lf::index{},
        const lf::state& state = lf::state{}
    );

    template<typename Container, typename ... Ptrs>
    Container make_unique_container(Ptrs&& ... ptrs) {
        Container result;
        ( result.emplace_back(std::forward<Ptrs>(ptrs)), ... );
        return result;
    }

}