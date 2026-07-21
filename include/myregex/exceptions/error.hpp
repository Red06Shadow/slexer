#ifndef MYREGEXERROR
#define MYREGEXERROR

#include <stdexcept>
#include <stdint.h>
#include <utilities/range.hpp>

namespace myregex
{
    enum class error_type : uint8_t
    {
        _S_syntax_error,
        _S_runtime_error,
        _S_input_error,
        _S_none
    };
    template <typename charT>
    class basic_regex_error : public std::exception
    {
    private:
        static std::basic_string<charT> _S_syntax_error_viewer(const basic_string_range<charT> &, size_t, size_t);

    private:
        inline static const char *_S_type_error[4] = {"syntax_error", "runtime_error", "input_error", "none"};
        std::basic_string<char> _M_message;
        std::basic_string<charT> _M_message_especification;
        error_type _M_type;
        uint8_t _M_id;

    public:
        explicit basic_regex_error() : std::exception(), _M_message(), _M_message_especification(), _M_type(error_type::_S_none), _M_id(0) {}
        explicit basic_regex_error(const std::basic_string<char> &, error_type, uint8_t);
        explicit basic_regex_error(const std::basic_string<char> &, size_t, const basic_string_range<charT> &, uint8_t);
        explicit basic_regex_error(const std::basic_string<char> &, size_t, size_t, const basic_string_range<charT> &, uint8_t);
        const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override { return _M_message.c_str(); }
        const charT *especification() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW { return _M_message_especification.c_str(); }
        virtual ~basic_regex_error() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {}
    };

    using regex_error = basic_regex_error<char>;
    using wregex_error = basic_regex_error<wchar_t>;

    template <typename charT>
    basic_regex_error<charT>::basic_regex_error(const std::basic_string<char> &message, error_type type, uint8_t id) : 
    std::exception(), _M_message("type: " + std::string(basic_regex_error<charT>::_S_type_error[static_cast<uint8_t>(type)]) + ":\n" + message  + "\ncode: " + std::to_string(id)), _M_message_especification(), _M_type(type), _M_id(id) {}
    
    
    template <typename charT>
    basic_regex_error<charT>::basic_regex_error(const std::basic_string<char> &message, size_t _index, const basic_string_range<charT> &_regular_expresion, uint8_t id) : 
    std::exception(), _M_message("type: " + std::string(basic_regex_error::_S_type_error[static_cast<uint8_t>(error_type::_S_syntax_error)]) + ":\n" + message  + "\ncode: " + std::to_string(id)), _M_message_especification(
        basic_regex_error<charT>::_S_syntax_error_viewer(_regular_expresion, _index, _index)
    ), _M_type(error_type::_S_syntax_error), _M_id(id) {}

    template <typename charT>
    basic_regex_error<charT>::basic_regex_error(const std::basic_string<char> &message, size_t _index_a , size_t _index_b, const basic_string_range<charT> &_regular_expresion, uint8_t id) : 
    std::exception(), _M_message("type: " + std::string(basic_regex_error::_S_type_error[static_cast<uint8_t>(error_type::_S_syntax_error)]) + ":\n" + message  + "\ncode: " + std::to_string(id)), _M_message_especification(
        basic_regex_error<charT>::_S_syntax_error_viewer(_regular_expresion, _index_a, _index_b)
    ), _M_type(error_type::_S_syntax_error), _M_id(id) {}

    template <typename charT>
    std::basic_string<charT> basic_regex_error<charT>::_S_syntax_error_viewer(const basic_string_range<charT> &expresion, size_t index_a, size_t index_b)
    {
        std::basic_string<charT> generate_spaces_and_mark = {};
        typename basic_string_range<charT>::iterator minrange;
        typename basic_string_range<charT>::iterator maxrange;

        if (expresion.begin() == expresion.end())
            return {};

        minrange = expresion.begin() + index_a;
        maxrange = expresion.begin() + index_b;

        for (typename basic_string_range<charT>::iterator i = expresion.begin(); i != expresion.end(); i++)
        {
            if (*i == charT('\n'))
                generate_spaces_and_mark.push_back(charT('\n'));
            else if (minrange <= i && i <= maxrange)
                generate_spaces_and_mark.push_back(charT('~'));
            else
                generate_spaces_and_mark.push_back(charT(' '));
        }
        if constexpr (std::is_same_v<charT, char>)
            return std::move("\nincident:\n" + std::basic_string<charT>(expresion.begin(), expresion.end()) + "\n" + generate_spaces_and_mark);
        else 
            return std::move(L"\nincident:\n" + std::basic_string<charT>(expresion.begin(), expresion.end()) + L"\n" + generate_spaces_and_mark);
    }
} // namespace myregex

#endif