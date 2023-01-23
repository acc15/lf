#pragma once

#include "log/sink/log_sink.hpp"
#include <filesystem>
#include <fstream>
#include <vector>
#include <string_view>

namespace lf {

    class rolling_file_sink: public log_sink {
        
        std::filesystem::path path;
        std::size_t max_file_size;
        std::size_t max_file_count;
        std::fstream file;
        bool add_date;

    public:

        struct rolling_file_info {
            std::filesystem::file_time_type time;
            std::string name;
            unsigned int seq;
        };

    private:
        
        std::vector<rolling_file_info> get_archive_files() const;
        std::filesystem::path next_archive_path(const std::vector<rolling_file_info>& archive_files) const;
        void write_archive(const std::filesystem::path& archive_path) const;
        void delete_redundant_files(std::vector<rolling_file_info>& vec) const;
        std::string filename_prefix() const;
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