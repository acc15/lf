#pragma once

#include "fs/serialization.hpp"
#include "io/log.hpp"

namespace lf {

    template <serializable T>
    struct change_state {

        T state;
        bool changed = false;

        void on(bool v) {
            changed |= v;
        }

        void load(const std::filesystem::path& p) {
            load_file(p, state);
        }

        void save_if_changed(const std::filesystem::path& p) const {
            if (changed) {
                save_file(p, state);
            } else {
                log.debug() && log() << "skipped save of " << T::name << " to " << p << " as it wasn't modified" << std::endl;
            }
        }

    };

}