#pragma once

#include "data_location.hpp"

namespace lf {
    class errors {
    public:

        data_location loc;

        errors(const data_location& loc);

        virtual ~errors() = default;

        size_t error_count() const;
        bool has_errors() const;

        void operator()(std::string_view msg);

    protected:
        virtual void on_error(std::string_view msg);

    private:
        size_t _count;

    };
} 
