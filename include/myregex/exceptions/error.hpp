#ifndef MYREGEXERROR
#define MYREGEXERROR

#include <stdexcept>
#include <myregex/utilities/range.hpp>

namespace myregex
{
    enum class error_type : unsigned char
    {
        _S_syntax_error,
        _S_runtime_error,
        _S_input_error
    };
    class regex_error : public std::runtime_error
    {
    private:
        std::string syntax_error_viewer(const stringrange& range, size_t a, size_t b);
        
    private:
        inline static const char* _M_name_error_type[3] = {"syntax_error\0", "runtime_error\0", "input_error\0"};
        myregex::error_type type : 2;
        unsigned char code : 6;

    public: // Averigurar por los errores para acdenas wchar_t
        explicit regex_error(const std::string &_message, myregex::error_type _type, unsigned char _code);
        explicit regex_error(const std::string &_message, size_t _index, const stringrange &_regular_expresion, unsigned char _code);
        explicit regex_error(const std::string &_message, size_t _index_a, size_t _index_b, const stringrange &_regular_expresion, unsigned char _code);
        virtual ~regex_error() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {}
    };

    regex_error::regex_error(const std::string &_message, myregex::error_type _type, unsigned char _code) : 
    std::runtime_error(
        "type: " + std::string(regex_error::_M_name_error_type[static_cast<unsigned char>(_type)]) + ":\n"
        + _message + "\ncode: " + std::to_string(_code)
    ), type(_type), code(_code) {}

    regex_error::regex_error(const std::string &_message, size_t _index, const stringrange &_regular_expresion, unsigned char _code) : 
    std::runtime_error(
        "\ntype: " + std::string(regex_error::_M_name_error_type[static_cast<unsigned char>(error_type::_S_syntax_error)]) + "\nmessage: "
        + _message + regex_error::syntax_error_viewer(_regular_expresion, _index, _index) + "\ncode: " + std::to_string(_code)
    ), type(error_type::_S_syntax_error), code(_code) {}

    regex_error::regex_error(const std::string &_message, size_t _index_a, size_t _index_b, const stringrange &_regular_expresion, unsigned char _code) : 
    std::runtime_error(
        "\ntype: " + std::string(regex_error::_M_name_error_type[static_cast<unsigned char>(error_type::_S_syntax_error)]) + "\nmessage: "
        + _message + regex_error::syntax_error_viewer(_regular_expresion, _index_a, _index_b) + "\ncode: " + std::to_string(_code)
    ), type(error_type::_S_syntax_error), code(_code) {}

    std::string regex_error::syntax_error_viewer(const stringrange& range, size_t a, size_t b) {
        std::string generate_spaces_and_mark = {};
        stringrange::iterator minrange;
        stringrange::iterator maxrange;

        if (range.begin() == range.end())
            return "";

        minrange = range.begin() + a;
        maxrange = range.begin() + b;

        for (stringrange::iterator i = range.begin(); i != range.end(); i++)
        {
            if (*i == '\n')
                generate_spaces_and_mark.push_back('\n');
            else if(minrange <= i && i <= maxrange)
                generate_spaces_and_mark.push_back('~');
            else
                generate_spaces_and_mark.push_back(' ');
        }

        return "\nincident:\n" + std::string(range.begin(), range.end()) + "\n" + generate_spaces_and_mark;
    }
} // namespace myregex

#endif