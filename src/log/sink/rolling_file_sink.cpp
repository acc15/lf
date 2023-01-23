#include "log/sink/rolling_file_sink.hpp"

#include <charconv>

#include "io/format_stream.hpp"
#include "io/time.hpp"

namespace fs = std::filesystem;

namespace lf {

    rolling_file_sink::rolling_file_sink(const std::filesystem::path& path, std::size_t max_file_size, std::size_t max_file_count, bool add_date): 
        path(path), 
        max_file_size(max_file_size),
        max_file_count(max_file_count),
        add_date(add_date)
    {
    }

    void rolling_file_sink::write(const log_message& message) {
        rollover();
        file << message.text;
        file.flush();
    }

    void rolling_file_sink::rollover() {

        if (!file.is_open()) {
            file.open(path, std::ios_base::in | std::ios_base::out | std::ios_base::app);
        }

        if (max_file_size == 0 || !fs::exists(path) || fs::file_size(path) < max_file_size) {
            return;
        }

        if (max_file_count > 0) {
            std::vector<rolling_file_info> archive_files = get_archive_files();
            fs::path archive_path = next_archive_path(archive_files);
            delete_redundant_files(archive_files);

            file.seekg(0);
            write_archive(archive_path);
        }

        // truncating and seeking to begin
        fs::resize_file(path, 0);
        file.clear();
        file.seekp(0);

    }

    void rolling_file_sink::write_archive(const std::filesystem::path& archive_path) const {
        std::ofstream a(archive_path, std::ios_base::out | std::ios_base::trunc);
        a << file.rdbuf();
    }

    std::vector<rolling_file_sink::rolling_file_info> rolling_file_sink::get_archive_files() const {
        const std::string log_name = path.filename().string();

        std::vector<rolling_file_info> result;
        for (const fs::directory_entry& e: fs::directory_iterator(path.parent_path())) {
            const fs::path path = e.path();
            const std::string f_name = path.filename().string();
            if (f_name.size() <= log_name.size()) {
                continue;
            }

            std::string::size_type last_dot_pos = f_name.find_last_of('.');
            if (last_dot_pos <= f_name.size()) {
                continue;
            }

            if (!f_name.starts_with(log_name)) {
                continue;
            }

            // TODO implement
            //unsigned int seq;
            //auto ec = std::from_chars(f_name.begin() + last_dot_pos, f_name.end(), seq);
            

        }
        return result;
    }


    std::string rolling_file_sink::filename_prefix() const {
        format_stream fmt;
        fmt << path.filename().string();
        if (add_date) {
            fmt << "." << format_date_time(std::chrono::system_clock::now(), false, "%Y_%m_%d"); 
        }
        fmt << ".";
        return fmt;
    }

    std::filesystem::path rolling_file_sink::next_archive_path(const std::vector<rolling_file_info>& archive_files) const {
        
        std::string prefix = filename_prefix();

        const fs::path parent = path.parent_path();
        fs::path result_path;

        size_t n = 1;
        do {
            std::string full_name = format_stream() << prefix << n;
            result_path = parent / full_name;
            ++n;
        } while (fs::exists(result_path));
        return result_path;
    }

    void rolling_file_sink::delete_redundant_files(std::vector<rolling_file_info>& archive_files) const {
        if (archive_files.size() < max_file_count) {
            return;
        }

        // TODO implement

        // size_t n = max_file_count - 1;
        // while (archive_files.size() > n) {
        //     archive_files.back().name
        // }

        // size_t n = map.size() - (max_file_count - 1);
        // const auto b = map.begin();
        // auto it = b;
        // do {
        //     fs::remove(b->second);
        //     ++it;
        //     --n;
        // } while (n > 0);
        // map.erase(b, it);
    }


}