#pragma once

#include "log/sink/log_sink.hpp"
#include "log/sink/rolling_file_info.hpp"

#include <filesystem>
#include <fstream>
#include <set>
#include <string_view>

namespace lf {

    class rolling_file_sink: public log_sink {
        
        std::filesystem::path path;
        std::size_t max_file_size;
        std::size_t max_file_count;
        std::fstream file;
        bool add_date;

        using info_set = std::set<rolling_file_info>;

        info_set get_archive_files() const;

        std::string get_archive_suffix() const;
        std::filesystem::path next_archive_path(const info_set& archive_files) const;

        void write_archive(const std::filesystem::path& archive_path) const;
        void delete_old_files(info_set& archive_files) const;
        void rollover();


    protected:
        void write(const log_message& message) override;

    public:
        rolling_file_sink(
            const std::filesystem::path& path, 
            std::size_t max_file_size, 
            std::size_t max_file_count, 
            bool add_date = true
        );


    };

}