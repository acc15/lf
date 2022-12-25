#pragma once

#include <sstream>

#include "data_location.hpp"

namespace lf {
    class errors {
    public:
        struct end_t {};
        static const end_t end;

        data_location loc;

        errors(const data_location& loc);
        virtual ~errors() = default;

        size_t error_count() const;
        bool has_errors() const;

        template <typename T>
        errors& operator<<(const T& value);

    protected:
        virtual void on_error(std::string_view msg);

    private:
        size_t _count;
        std::stringstream _stream;

    };

    template <typename T>
    errors& errors::operator<<(const T& value) {
        _stream << value;
        return *this;
    }

    template <>
    errors& errors::operator<<(const end_t&);

} 
