#pragma once

namespace lf {

    template<typename Container, typename ... Ptrs>
    Container make_unique_container(Ptrs&& ... ptrs) {
        Container result;
        ( result.emplace_back(std::forward<Ptrs>(ptrs)), ... );
        return result;
    }

}