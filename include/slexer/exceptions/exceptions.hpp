#ifndef SLEXEREXCEPTION
#define SLEXEREXCEPTION

#include <slexer/token/token.hpp>
#include <utilities/range.hpp>
#include <slexer/buffer/buffer.hpp>
//<--...
#include <stdexcept>
#include <stdint.h>

namespace slexer
{
    class critical_error : public std::logic_error
    {
    private:
        unsigned int _M_code;

    public:
        critical_error(uint32_t __code, const std::string &__message)
            : std::logic_error("critical error in lexer: " +
                               __message + " code: " +
                               std::to_string(__code)),
              _M_code(__code) {}
        inline unsigned char code() const { return _M_code; }
        ~critical_error() {}
    };

    template <typename charT, typename idT>
    class basic_lexical_error : public std::exception
    {
    public:
        static std::basic_string<charT> _S_syntax_error_viewer(const basic_string_range<charT> &, size_t, size_t);
        typedef std::basic_string<charT> (*handle)(uint16_t, const std::basic_string<char> &, std::basic_istream<charT>&, const slexer::basic_token<charT, idT> &);

    private:
        uint16_t _M_code;
        std::basic_string<char> _M_message;
        slexer::basic_token<charT, idT> _M_capture;
        handle _M_handle;

    public:
        basic_lexical_error(uint16_t, const std::basic_string<char> &, const slexer::basic_token<charT, idT> &);
        basic_lexical_error(uint16_t, const std::basic_string<char> &, const slexer::basic_token<charT, idT> &, basic_lexical_error<charT, idT>::handle);
        inline uint16_t code() const { return _M_code; }
        inline const std::string& message() const _GLIBCXX_NOTHROW { return _M_message; }
        inline std::basic_string<charT> especification(std::basic_istream<charT>& in) const _GLIBCXX_NOTHROW { return _M_handle(_M_code, _M_message, in, _M_capture); }
        inline const slexer::basic_token<charT, idT>& capture() const { return _M_capture; }
        inline const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override { return _M_message.c_str(); }
        ~basic_lexical_error() {}
    };
    template <typename charT, typename idT>
    basic_lexical_error<charT, idT>::basic_lexical_error(uint16_t _code, const std::basic_string<char> &__message, const slexer::basic_token<charT, idT> &__capture) : std::exception(), _M_code(_code), _M_message(__message), _M_capture(__capture), _M_handle(nullptr) {}
    template <typename charT, typename idT>
    basic_lexical_error<charT, idT>::basic_lexical_error(uint16_t _code, const std::basic_string<char> &__message, const slexer::basic_token<charT, idT> &__capture, basic_lexical_error<charT, idT>::handle __handle) : std::exception(), _M_code(_code), _M_message(__message), _M_capture(__capture), _M_handle(__handle) {}
    /// @brief Funcion de captura por defecto
    template <typename charT, typename idT>
    inline constexpr typename basic_lexical_error<charT, idT>::handle default_hanlde_excpetion = [](uint16_t _code, const std::basic_string<char> & _message, std::basic_istream<charT>& in, const slexer::basic_token<charT, idT> & tokn) -> std::basic_string<charT> {
        in.seekg(tokn.position() - tokn.column());
        std::basic_string<charT> line;
        std::getline(in, line);
        basic_string_range<charT> range {line};
        std::basic_string<charT> especification = "in [line: " + std::to_string(tokn.line()) + ", column: " + std::to_string(tokn.column()) + ", position: " + std::to_string(tokn.position()) + "]: " + tokn.text() + "\n\t" + line + "\n\t" + basic_lexical_error<charT, idT>::_S_syntax_error_viewer(range, tokn.column(), tokn.column() + (tokn.text().size() - 1));
        return especification;
    };

    template <typename charT, typename idT>
    std::basic_string<charT> basic_lexical_error<charT, idT>::_S_syntax_error_viewer(const basic_string_range<charT> &expresion, size_t index_a, size_t index_b)
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
            return generate_spaces_and_mark;
        else 
            return generate_spaces_and_mark;
    }
} // namespace slexer
//<--...

#endif