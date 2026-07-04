#ifndef SLEXEREXCEPTION
#define SLEXEREXCEPTION

#include <stdexcept>
#include <slexer/token/token.hpp>

namespace slexer
{
    class critical_error : public std::logic_error
    {
    private:
        unsigned int _M_code;

    public:
        critical_error(const std::string &__message, unsigned int __code)
            : std::logic_error("critical error in lexer: " +
                               __message + " code: " +
                               std::to_string(__code)),
              _M_code(__code) {}
        inline unsigned char code() const { return _M_code; }
        ~critical_error() {}
    };

    template <typename charT, typename idT>
    class basic_lexical_error
    {
    public:
        typedef void (*handle)(slexer::basic_token<charT, idT> &, std::basic_string<charT> &);

    private:
        std::basic_string<charT> _M_message;
        slexer::basic_token<charT, idT> _M_capture;
        handle _M_handle;

    public:
        basic_lexical_error(const std::basic_string<charT> &__message, const slexer::basic_token<charT, idT> &__capture) : _M_message(__message), _M_capture(__capture), _M_handle(nullptr) {}
        basic_lexical_error(const std::basic_string<charT> &__message, const slexer::basic_token<charT, idT> &__capture, basic_lexical_error<charT, idT>::handle __handle) : _M_message(__message), _M_capture(__capture), _M_handle(__handle) {}
        basic_lexical_error(const basic_lexical_error &other) : _M_message(other._M_message), _M_capture(other._M_capture), _M_handle(other._M_handle) {}
        const charT *
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW
        {
            if (_M_handle)
                _M_handle(_M_capture, _M_message);
            return _M_message.cstr();
        }
        ~basic_lexical_error() {}
    };
} // namespace slexer

#endif