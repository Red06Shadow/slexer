#ifndef SLEXERTOKEN
#define SLEXERTOKEN

#include <string>

namespace slexer
{
    template <typename charT, typename idT>
    class basic_lexer;

    template <typename charT, typename idT>
    class basic_token
    {
    private:
        static_assert(std::is_unsigned_v<idT> | std::is_enum_v<idT>, "in basic_token: do you no have template with value is not a unsigned integer");
        std::basic_string<charT> _M_text;
        idT _M_id;
        size_t _M_line, _M_position, _M_column;

    public:
        basic_token() : _M_text(), _M_id(), _M_line(0), _M_position(0), _M_column(0) {}
        basic_token(const std::basic_string<charT>& text, idT _id, size_t _position, size_t _line, size_t _column) : 
        _M_text(text), _M_id(_id), _M_line(_line), _M_position(_position), _M_column(_column) {}
        basic_token(std::basic_string<charT>&& text, size_t _id, size_t _position, size_t _line, size_t _column) : 
        _M_text(text), _M_id(_id), _M_line(_line), _M_position(_position), _M_column(_column) {}
        inline std::basic_string<charT>& text() { return _M_text; }
        inline const std::basic_string<charT>& text() const { return _M_text; }
        inline idT id() const { return _M_id; }
        inline idT& id() { return _M_id; }
        ///////////////////////////////////////////////
        inline size_t position() const { return _M_position; }
        inline size_t line() const { return _M_line; }
        inline size_t column() const { return _M_column; }
        inline size_t& position() { return _M_position; }
        inline size_t& line() { return _M_line; }
        inline size_t& column() { return _M_column; }
        void view() const { std::selector<charT>::stream() << charT('{') << std::endl << charT('\t') << "str: "<< _M_text << std::endl << charT('\t') << "id: "<< _M_id << std::endl << charT('\t') << "pos: "<< _M_position << std::endl << charT('\t') << "line: "<< _M_line << std::endl << charT('\t') << "column: "<< _M_column << std::endl << charT('}') << std::endl; }
        ~basic_token() {}
        friend basic_lexer<charT, idT>;
    };

    template <typename idT>
    using token = basic_token<char, idT>;
    template <typename idT>
    using wtoken = basic_token<wchar_t, idT>;
    
} // namespace slexer


#endif