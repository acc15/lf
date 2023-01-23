#include "log/sink/rolling_file_sink.hpp"

namespace lf {

    rolling_file_sink::rolling_file_sink(const std::filesystem::path& path, std::size_t max_file_size, std::size_t max_file_count): 
        path(path), 
        max_file_size(max_file_size),
        max_file_count(max_file_count)
    {
    }

    void rolling_file_sink::write(const log_message& message) {
        
    }

}