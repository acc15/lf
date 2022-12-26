#pragma once

#include <sstream>

namespace lf {
    class errors {
    public:
        struct end_t {};
        static const end_t end;

        errors(const std::string& loc);
        
        errors(const errors& c);
        errors(errors&& c);

        size_t error_count() const;
        bool has_errors() const;

        template <typename T>
        errors& operator<<(const T& value);

    private:
        void on_error(std::string_view msg);
        std::stringstream _stream;
        std::string _loc;
        size_t _count;

    };

    template <typename T>
    errors& errors::operator<<(const T& value) {
        _stream << value;
        return *this;
    }

    template <>
    errors& errors::operator<<(const end_t&);

} 
