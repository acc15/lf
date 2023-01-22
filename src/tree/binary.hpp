#pragma once

#include "tree/tree.hpp"
#include "io/format.hpp"
#include "log/log.hpp"

#include <cstring>
#include <algorithm>
#include <iostream>
#include <vector>
#include <concepts>

namespace lf {

    template <typename Tree>
    concept serializable_tree_concept = tree_concept<Tree> && requires {
        { Tree::file_signature } -> std::convertible_to<const char*>;
        { Tree::file_version } -> std::convertible_to<std::uint8_t>;
    };

    template <serializable_tree_concept Tree>
    struct tree_binary {

        using tree_type = Tree;
        using map_type = typename tree_type::map_type;
        using nested_tree = typename map_type::mapped_type;
        using entry_map_ptr = typename map_type::const_pointer;
        using entry_map_ref = typename map_type::const_reference;

        static void add_entries(const map_type& entries, std::vector<entry_map_ptr>& queue) {
            std::for_each(entries.begin(), entries.end(), [&queue](entry_map_ref e) { queue.push_back(&e); });
        }

        static std::ostream& write(std::ostream& s, const tree_type& tree) {
            
            if (!(s << tree_type::file_signature)) {
                log.error() && log() << "unable to write file signature: " << strerror(errno) << log::end;
                return s;
            }

            if (!(s << tree_type::file_version)) {
                log.error() && log() << "unable to write file version: " << strerror(errno) << log::end;
                return s;
            }

            if (!(s << with_ref_format<format::BINARY>(tree.data))) {
                log.error() && log() << "unable to write root data: " << strerror(errno) << log::end;
                return s;
            }

            std::vector<entry_map_ptr> queue;
            add_entries(tree.entries, queue);

            while (!queue.empty()) {
                const auto e = queue.back();
                queue.pop_back();

                if (e == nullptr) {
                    log.trace() && log() << "entry ended, writing zero char" << log::end;
                    s << '\0';
                    continue;
                }

                log.trace() && log() << "writing entry " << e->first << "..." << log::end;
                if (!(s << e->first << '\0')) {
                    log.error() && log() << "unable to write entry " << e->first << " name: " << strerror(errno) << log::end;
                    break;
                }

                if (!(s << with_ref_format<format::BINARY>(e->second.data))) {
                    log.error() && log() << "unable to write entry " << e->first << " data: " << strerror(errno) << log::end;
                    break;
                }

                if (e->second.entries.empty()) {
                    
                    if (std::all_of(queue.begin(), queue.end(), [](auto p) { return p == nullptr; })) {
                        log.trace() && log() << "all other entries are zeroes... ending tree file" << log::end;
                        break;
                    }

                    log.trace() && log() << "entry " << e->first << " is empty, writing zero char" << log::end;
                    if (!(s << '\0')) {
                        log.error() && log() << "unable to write entry " << e->first << " finising zero char: " << strerror(errno) << log::end;
                        break;
                    }

                    continue;
                }

                queue.push_back(nullptr);
                add_entries(e->second.entries, queue);

            }
            return s;
        }

        static std::istream& read(std::istream& s, tree_type& tree) {
            
            tree.data = {};
            tree.entries.clear();

            char signature[std::size(tree_type::file_signature)];
            uint8_t version;

            if (!(s >> signature)) {
                log.error() && log() << "unable to read file signature: " << strerror(errno) << log::end;
                return s;
            }
            
            if (std::strcmp(signature, tree_type::file_signature) != 0) {
                log.error() && log() << "invalid file signature: " << signature << log::end;
                s.setstate(std::istream::failbit);
                return s;
            }

            if (!(s >> version)) {
                log.error() && log() << "unable to read file version: " << strerror(errno) << log::end;
                return s;
            }

            if (version != tree_type::file_version) {
                log.error() && log() << "invalid file version: " << static_cast<unsigned int>(version) << log::end;
                s.setstate(std::istream::failbit);
                return s;
            }

            if (!(s >> with_ref_format<format::BINARY>(tree.data))) {
                log.error() && log() << "unable to read root data: " << strerror(errno) << log::end;
                return s;
            }

            std::vector<map_type*> stack;
            stack.push_back(&tree.entries);

            do {

                std::istream::int_type next = s.peek();
                if (next == std::istream::traits_type::eof()) {
                    break;
                }

                if (next == 0) {
                    s.get();
                    stack.pop_back();
                    continue;
                }
                
                std::string name;
                if (!getline(s, name, '\0')) {
                    log.error() && log() << "unable to read entry name: " << strerror(errno) << log::end;
                    break;
                }

                nested_tree& entry = (*stack.back())[name];
                if (!(s >> with_ref_format<format::BINARY>(entry.data))) {
                    log.error() && log() << "unable to read entry " << name << " data: " << strerror(errno) << log::end;
                    break;
                }

                stack.push_back(&entry.entries);

            } while (!stack.empty());

            return s;
        }

    };

    template <serializable_tree_concept Tree>
    std::ostream& operator<<(std::ostream& s, with_format<format::BINARY, const Tree&> tree) {
        return tree_binary<Tree>::write(s, tree.value);
    }
    
    template <serializable_tree_concept Tree>
    std::istream& operator>>(std::istream& s, with_format<format::BINARY, Tree&> tree) {
        return tree_binary<Tree>::read(s, tree.value);
    }

}