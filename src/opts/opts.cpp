#include "opts/opts.hpp"

#include "log/log.hpp"

namespace lf {

    const opt* opts::find_by_name(std::string_view name) const {
        for (const opt& o: opts) {
            if (o.name == name) {
                return &o;
            }
        }
        log.warn() && log() << "unknown option: " << name << log::end;
        return nullptr;
    }

    const opt* opts::find_by_alias(std::string_view alias) const {
        if (alias.size() == 1) {
            for (const opt& o: opts) {
                if (o.alias != 0 && o.alias == alias[0]) {
                    return &o;
                }
            }
        }
        log.warn() && log() << "unknown option alias: " << alias << log::end;
        return nullptr;
    }

    opt_map opts::parse(const std::span<const char*>& args) const {
        opt_map::map_type map;

        const opt* last_opt = nullptr;
        for (const char* arg_ptr: args) {
            std::string_view arg = arg_ptr;
            
            bool is_opt = false;
            if (arg.starts_with("--")) {
                arg.remove_prefix(2);
                last_opt = find_by_name(arg);
                is_opt = true;
            } else if (arg.starts_with('-')) {
                arg.remove_prefix(1);
                last_opt = find_by_alias(arg);
                is_opt = true;
            }

            if (last_opt != nullptr) {
                opt_map::vec_type& v = map[last_opt->name];
                if (last_opt->max_occurs != opt::UNBOUNDED && v.size() >= last_opt->max_occurs) {
                    last_opt = nullptr;
                }
            }

            if (is_opt) {
                continue;
            }

            if (arg.starts_with('\\')) {
                arg.remove_prefix(1);
            }

            std::string_view name = last_opt != nullptr ? last_opt->name : default_opt;
            map[name].push_back(arg);
        }

        return opt_map(std::move(map));
    }

    std::ostream& operator<<(std::ostream& s, const opts& v) {
        for (const auto& o: v.opts) {
            s << "    ";
            if (o.alias != 0) {
                s << "-" << o.alias;
            }
            if (!o.name.empty()) {
                if (o.alias != 0) {
                    s << ", ";
                }
                s << "--" << o.name;
            }
            if (!o.arg_description.empty()) {
                if (!o.name.empty()) {
                    s << ' ';
                }
                s << o.arg_description;
            }
            if (!o.description.empty()) {
                s << " - " << o.description;
            }
            s << std::endl;
        }
        return s;
    }

}