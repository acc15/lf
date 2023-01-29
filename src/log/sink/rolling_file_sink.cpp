#include "log/sink/rolling_file_sink.hpp"
#include "io/time.hpp"

#include <algorithm>

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
        if (max_file_size == 0 || !fs::exists(path) || fs::file_size(path) < max_file_size) {
            if (!file.is_open()) {
                fs::create_directories(path.parent_path());
                file.open(path, std::ios_base::app);
            }
            return;
        }

        if (file.is_open()) {
            file.close();
        }

        if (max_file_count > 0) {
            info_set archive_files = get_archive_files();
            fs::path archive_path = next_archive_path(archive_files);
            delete_old_files(archive_files);
            fs::rename(path, archive_path);
        }

        file.open(path, std::ios_base::trunc);

    }

    void rolling_file_sink::write_archive(const std::filesystem::path& archive_path) const {
        std::ofstream a(archive_path, std::ios_base::out | std::ios_base::trunc);
        a << file.rdbuf();
    }

    rolling_file_sink::info_set rolling_file_sink::get_archive_files() const {
        const std::string log_name = path.filename().string();

        info_set result;
        for (const fs::directory_entry& e: fs::directory_iterator(path.parent_path())) {
            const fs::path p = e.path();
            const std::string f_name = p.filename().string();
            std::optional<rolling_file_info> f_info = rolling_file_info::parse(f_name, log_name);
            if (!f_info.has_value()) {
                continue;
            }
            f_info->time = e.last_write_time();
            result.insert(f_info.value());
        }

        return result;
    }

    std::string rolling_file_sink::get_archive_suffix() const {
        std::ostringstream fmt;
        if (add_date) {
            fmt << format_date_time(std::chrono::system_clock::now(), false, ".%Y_%m_%d");
        }
        fmt << ".";
        return fmt.str();
    }

    std::filesystem::path rolling_file_sink::next_archive_path(const info_set& archive_files) const {
        
        rolling_file_info fi;
        fi.name = get_archive_suffix();
        fi.seq = 1;
        for (const rolling_file_info& i: archive_files) {
            if (i.name == fi.name && i.seq >= fi.seq) {
                fi.seq = i.seq + 1;
            }
        }
        return fi.make_path(path);
    }

    void rolling_file_sink::delete_old_files(info_set& archive_files) const {
        if (archive_files.size() < max_file_count) {
            return;
        }

        size_t remove_count = archive_files.size() - (max_file_count - 1);
        const auto b = archive_files.begin();
        auto e = b;
        do {
            fs::remove(e->make_path(path));
            ++e;
            --remove_count;
        } while (remove_count > 0);
        archive_files.erase(b, e);
    }


}