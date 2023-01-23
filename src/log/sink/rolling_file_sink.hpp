#pragma once

#include "log/sink/log_sink.hpp"
#include <filesystem>
#include <fstream>

namespace lf {

    class rolling_file_sink: public log_sink {
        std::filesystem::path path;
        std::size_t max_file_size;
        std::size_t max_file_count;
        std::ofstream file;

    protected:
        void write(const log_message& message) override;

    public:
        rolling_file_sink(const std::filesystem::path& path, std::size_t max_file_size, std::size_t max_file_count);

    };

}