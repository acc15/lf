#pragma once

#include "tree/tree.hpp"
#include "io/format.hpp"
#include "io/util.hpp"
#include "log/log.hpp"

#include <cstring>
#include <algorithm>
#include <ostream>
#include <vector>

namespace lf {

    struct tree_binary_format: format<true> {};

    template <serializable_tree_type Tree>
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
                log.error() && log() 
                    << "unable to write file signature: " << get_errno_message() << log::end;
                return s;
            }

            if (!(s << tree_type::file_version)) {
                log.error() && log() 
                    << "unable to write file version: " << get_errno_message() << log::end;
                return s;
            }

            if (!(s << write_as<tree_binary_format>(tree.data))) {
                log.error() && log() 
                    << "unable to write root data: " << get_errno_message() << log::end;
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
                    log.error() && log() 
                        << "unable to write entry " << e->first << " name: " << get_errno_message() << log::end;
                    break;
                }

                if (!(s << write_as<tree_binary_format>(e->second.data))) {
                    log.error() && log() 
                        << "unable to write entry " << e->first << " data: " << get_errno_message() << log::end;
                    break;
                }

                if (e->second.entries.empty()) {
                    
                    if (std::all_of(queue.begin(), queue.end(), [](auto p) { return p == nullptr; })) {
                        log.trace() && log() << "all other entries are zeroes... ending tree file" << log::end;
                        break;
                    }

                    log.trace() && log() << "entry " << e->first << " is empty, writing zero char" << log::end;
                    if (!(s << '\0')) {
                        log.error() && log() 
                            << "unable to write entry " << e->first 
                            << " finising zero char: " << get_errno_message() << log::end;
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
                log.error() && log() 
                    << "unable to read file signature: " << get_errno_message() << log::end;
                return s;
            }
            
            if (std::strcmp(signature, tree_type::file_signature) != 0) {
                log.error() && log() << "invalid file signature: " << signature << log::end;
                s.setstate(std::istream::failbit);
                return s;
            }

            if (!(s >> version)) {
                log.error() && log() 
                    << "unable to read file version: " << get_errno_message() << log::end;
                return s;
            }

            if (version != tree_type::file_version) {
                log.error() && log() 
                    << "invalid file version: " << static_cast<unsigned int>(version) << log::end;
                s.setstate(std::istream::failbit);
                return s;
            }

            if (!(s >> read_as<tree_binary_format>(tree.data))) {
                log.error() && log() 
                    << "unable to read root data: " << get_errno_message() << log::end;
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
                    log.error() && log() 
                        << "unable to read entry name: " << get_errno_message() << log::end;
                    break;
                }

                nested_tree& entry = (*stack.back())[name];
                if (!(s >> read_as<tree_binary_format>(entry.data))) {
                    log.error() && log() 
                        << "unable to read entry " << name << " data: " << get_errno_message() << log::end;
                    break;
                }

                stack.push_back(&entry.entries);

            } while (!stack.empty());

            return s;
        }

    };

    template <serializable_tree_type Tree>
    std::ostream& operator<<(std::ostream& s, with_format<tree_binary_format, const Tree> tree) {
        return tree_binary<Tree>::write(s, tree.value);
    }
    
    template <serializable_tree_type Tree>
    std::istream& operator>>(std::istream& s, with_format<tree_binary_format, Tree> tree) {
        return tree_binary<Tree>::read(s, tree.value);
    }

}