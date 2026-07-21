#ifndef SLEXERMAIN
#define SLEXERMAIN

#include <utilities/memory.hpp>
#include <myregex/automaton/table.hpp>
#include <slexer/buffer/buffer.hpp>
#include <slexer/token/token.hpp>
#include <slexer/tokenstream/tokenstream.hpp>
#include <slexer/exceptions/exceptions.hpp>
//<--...
#include <iostream>
#include <fstream>
#include <filesystem>
#include <queue>


namespace slexer
{
    template <typename charT, typename idT>
    class basic_builder;

    enum class Flags : unsigned int
    {
        _S_emit_error = 0x0001,
        _S_emit_token = 0x0002,
        _S_push_capture = 0x0004,

    };
    _GLIBCXX_NODISCARD _GLIBCXX_CONSTEXPR inline Flags
    operator&(Flags __a, Flags __b) _GLIBCXX_NOTHROW { return Flags(static_cast<unsigned int>(__a) & static_cast<unsigned int>(__b)); }
    _GLIBCXX_NODISCARD _GLIBCXX_CONSTEXPR inline Flags
    operator|(Flags __a, Flags __b) _GLIBCXX_NOTHROW { return Flags(static_cast<unsigned int>(__a) | static_cast<unsigned int>(__b)); }
    _GLIBCXX_NODISCARD _GLIBCXX_CONSTEXPR inline Flags
    operator^(Flags __a, Flags __b) _GLIBCXX_NOTHROW { return Flags(static_cast<unsigned int>(__a) ^ static_cast<unsigned int>(__b)); }
    _GLIBCXX14_CONSTEXPR
    inline const Flags &
    operator&=(Flags &__a, Flags __b) _GLIBCXX_NOTHROW { return __a = __a & __b; }
    _GLIBCXX14_CONSTEXPR
    inline const Flags &
    operator|=(Flags &__a, Flags __b) _GLIBCXX_NOTHROW { return __a = __a | __b; }
    _GLIBCXX14_CONSTEXPR
    inline const Flags &
    operator^=(Flags &__a, Flags __b) _GLIBCXX_NOTHROW { return __a = __a ^ __b; }

    template <typename charT, typename idT>
    class basic_lexer
    {
    public:
        static_assert(std::is_unsigned_v<idT> | std::is_enum_v<idT>, "in basic_lexer: do you no have template with value is not a unsigned integer");
        class master
        {
        private:
            /// @brief Inicio de tabla
            size_t _M_begin;
            Flags _M_flags;
            size_t _M_size_group;

        public:
            slexer::basic_token<charT, idT> _capture;
            std::queue<slexer::basic_token<charT, idT>> _queue_emit;

        public:
            master(size_t _group_size, size_t _buffer_size) : _M_size_group(_group_size), _M_begin(0), _M_flags(Flags::_S_emit_token | Flags::_S_push_capture) { _capture._M_text.reserve(_buffer_size); }
            inline void begin(size_t _begin)
            {
                if (_begin >= _M_size_group)
                    throw slexer::critical_error("no access to grupo over-size to table", 1);
                _M_begin = _begin;
            }
            inline void setflag(Flags _flag)
            {
                if (!(unsigned int)(_flag & _M_flags))
                    _M_flags ^= _flag;
            }
            inline void unsetflag(Flags _flag)
            {
                if ((unsigned int)(_flag & _M_flags))
                    _M_flags ^= _flag;
            }
            inline bool getflag(Flags _flag)
            {
                return (unsigned int)(_flag & _M_flags);
            }
            friend basic_lexer;
        };

        /// @brief Patron de la funcion de captura
        typedef void (*handle)(basic_lexer<charT, idT>::master &);
        /// @brief Valor por defecto de inicio
        inline static constexpr size_t main = 0;
        /// @brief Funcion de captura por defecto
        static inline constexpr basic_lexer<charT, idT>::handle defaultf = [](basic_lexer<charT, idT>::master &) -> void {};

    public:
        class _I_idT
        {
        private:
            idT _M_id;
            handle _M_handle;

        public:
            _I_idT() : _M_id(), _M_handle(nullptr) {}
            _I_idT(idT _id, handle _handle) : _M_id(_id), _M_handle(_handle) {}
            inline idT id() const { return _M_id; }
            inline handle funt() const { return _M_handle; }
            friend bool operator<(const _I_idT &lhs, const _I_idT &rhs) { return lhs._M_id < rhs._M_id; }
            friend std::ostream &operator<<(std::ostream &out, const _I_idT &element)
            {
                // out << _M_association_id;
                return out;
            }
            ~_I_idT() {}
        };

    private:
        /// @brief Tablas de expreseiones
        std::basic_allocator<myregex::basic_table<charT, _I_idT>> _M_group_tables;
        /// @brief Buffer de lectura
        slexer::basic_buffer<charT> _M_buffer_input;

        /// @brief Captura de posicion
        size_t _M_capture_position;
        /// @brief Captura de linea
        size_t _M_capture_line;
        /// @brief Captura de columna
        size_t _M_capture_column;

    private: /// @brief Funciones privadass
        /// @brief
        /// @param
        /// @param
        /// @param
        void _M_caption(std::basic_ifstream<charT> &, master &, basic_lexer::handle &);
        void _M_caption(typename std::basic_string<charT>::const_iterator &, const typename std::basic_string<charT>::const_iterator &, master &, basic_lexer::handle &);
        /// @brief Contador de posicion de archivo
        /// @param letter caracter actual
        inline void _M_manager_position(charT letter);
        constexpr inline static size_t _S_value(charT caracter)
        {
            size_t value;
            if constexpr (std::is_same_v<charT, char>)
                return (unsigned char)(caracter);
            else
            {
                if constexpr (sizeof(wchar_t) == 2)
                    return (unsigned short)(caracter);
                else
                    return (unsigned int)(caracter);
            }
        }

    public:
        /// @brief
        /// @param group
        /// @param size_buffer
        basic_lexer(std::basic_allocator<myregex::basic_table<charT, _I_idT>> &&group, unsigned int size_buffer)
            : _M_group_tables(std::move(group)),
              _M_buffer_input(size_buffer),
              _M_capture_line(0),
              _M_capture_column(0),
              _M_capture_position(0) {}
        /// @brief
        /// @param other
        basic_lexer(const basic_lexer<charT, idT> &other) = delete;

        /// @brief Constructor de movimiento base
        /// @param other otro objecto basic_lexer
        basic_lexer(basic_lexer<charT, idT> &&other)
            : _M_group_tables(std::move(other._M_group_tables)),
              _M_buffer_input(std::move(other._M_buffer_input)),
              _M_capture_line(other._M_capture_line),
              _M_capture_column(other._M_capture_column),
              _M_capture_position(other._M_capture_position) {}

        /// @brief
        /// @param other
        /// @return
        basic_lexer<charT, idT> &operator=(const basic_lexer<charT, idT> &other) = delete;

        /// @brief
        /// @param other
        /// @return
        basic_lexer<charT, idT> &operator=(basic_lexer<charT, idT> &&other);

        /// @brief
        /// @return
        slexer::basic_tokenstream<charT, idT> tokenize(std::basic_ifstream<charT> &_M_stream);

        slexer::basic_tokenstream<charT, idT> tokenize(const std::basic_string<charT> &_M_stream);

        size_t size() const
        {
            size_t size_ = 0;
            for (size_t i = 0; i < _M_group_tables.size(); i++)
                size_ = _M_group_tables[i].size();
            return size_;
        }

        void compiler(const std::filesystem::path &path);

        friend basic_builder<charT, idT>;
    };

    template <typename charT, typename idT>
    basic_lexer<charT, idT> &basic_lexer<charT, idT>::operator=(basic_lexer<charT, idT> &&other)
    {
        if (this != &other)
        {
            _M_group_tables = std::move(other._M_group_tables);
            _M_buffer_input = std::move(other._M_buffer_input);
            _M_capture_line = std::move(other._M_capture_line);
            _M_capture_column = std::move(other._M_capture_column);
            _M_capture_position = std::move(other._M_capture_position);
        }
        return *this;
    }
    template <typename charT, typename idT>
    void basic_lexer<charT, idT>::_M_manager_position(charT letter)
    {
        this->_M_capture_column++;
        if (letter == charT('\n'))
        {
            this->_M_capture_column = 0;
            this->_M_capture_line++;
        }
        this->_M_capture_position++;
    }
    template <typename charT, typename idT>
    void basic_lexer<charT, idT>::_M_caption(std::basic_ifstream<charT> &_M_stream, master &_M_master, basic_lexer<charT, idT>::handle &_M_handle)
    {
        bool acceptance = false;
        size_t status = 0;
        size_t acceptance_status = -1ULL;
        _I_idT id{};
        std::basic_string<charT> str{};
        while (_M_buffer_input.position() < _M_buffer_input.size())
        {
            charT letter = _M_buffer_input.peak();
            size_t next = _M_group_tables[_M_master._M_begin].transitions()[status * myregex::basic_table<charT, _I_idT>::dictionary + _S_value(letter)];
            if (next == -1ULL)
                break; // No hay transición, rechazar
            status = next;
            str.push_back(letter);
            if (status == acceptance_status)
                goto _M_jump_if_status_is_acceptance;
            else if (_M_group_tables[_M_master._M_begin].status()[status].valid())
            {
                acceptance_status = status;
                id = _M_group_tables[_M_master._M_begin].status()[status].get();
                acceptance = true;
            }
        _M_jump_if_status_is_acceptance:
            _M_buffer_input.next();
            _M_manager_position(letter);
            if (_M_buffer_input.position() >= _M_buffer_input.size() && !_M_buffer_input.eof())
                _M_stream >> _M_buffer_input;
        }
        _M_master._capture._M_text = std::move(str);
        _M_master._capture._M_id = id.id();
        if (acceptance)
            _M_handle = id.funt();
        else
            _M_master.setflag(Flags::_S_emit_error);
    }
    template <typename charT, typename idT>
    slexer::basic_tokenstream<charT, idT> basic_lexer<charT, idT>::tokenize(std::basic_ifstream<charT> &_M_stream)
    {
        slexer::basic_tokenstream<charT, idT> _M_basic_tokenstream;
        master _M_master{_M_group_tables.size(), _M_buffer_input.size()};
        _M_stream >> _M_buffer_input;
        while (_M_buffer_input.position() < _M_buffer_input.size())
        {
            handle _M_handle = nullptr;
            _M_master._capture._M_position = _M_capture_position;
            _M_master._capture._M_line = _M_capture_line;
            _M_master._capture._M_column = _M_capture_column;
            basic_lexer::_M_caption(_M_stream, _M_master, _M_handle);
            if (_M_master.getflag(Flags::_S_emit_error))
            {
                if constexpr (std::is_same_v<charT, char>)
                    throw slexer::basic_lexical_error<charT, idT>("token inseperado o mal formado", _M_master._capture);
                else
                    throw slexer::basic_lexical_error<charT, idT>(L"token inseperado o mal formado", _M_master._capture);
            }
            if (_M_handle == nullptr)
                continue;
            _M_handle(_M_master);
            if (_M_master.getflag(Flags::_S_push_capture))
                _M_master._queue_emit.push(_M_master._capture);
            if (!_M_master.getflag(Flags::_S_emit_token))
                continue;
            while (!_M_master._queue_emit.empty())
            {
                _M_basic_tokenstream.push(std::move(_M_master._queue_emit.front()));
                _M_master._queue_emit.pop();
            }
        }
        return _M_basic_tokenstream;
    }

    template <typename charT, typename idT>
    void basic_lexer<charT, idT>::_M_caption(typename std::basic_string<charT>::const_iterator &_M_string, const typename std::basic_string<charT>::const_iterator &_M_end, master &_M_master, basic_lexer::handle &_M_handle)
    {
        bool acceptance = false;
        size_t status = 0;
        size_t acceptance_status = -1ULL;
        _I_idT id{};
        std::basic_string<charT> str{};
        while (_M_string < _M_end)
        {
            charT letter = *_M_string;
            size_t next = _M_group_tables[_M_master._M_begin].transitions()[status * myregex::basic_table<charT, _I_idT>::dictionary + _S_value(letter)];
            if (next == -1ULL)
                break; // No hay transición, rechazar
            status = next;
            str.push_back(letter);
            if (status == acceptance_status)
                goto _M_jump_if_status_is_acceptance;
            else if (_M_group_tables[_M_master._M_begin].status()[status].valid())
            {
                acceptance_status = status;
                id = _M_group_tables[_M_master._M_begin].status()[status].get();
                acceptance = true;
            }
        _M_jump_if_status_is_acceptance:
            _M_string++;
            _M_manager_position(letter);
        }
        _M_master._capture._M_text = std::move(str);
        _M_master._capture._M_id = id.id();
        if (acceptance)
            _M_handle = id.funt();
        else
            _M_master.setflag(Flags::_S_emit_error);
    }

    template <typename charT, typename idT>
    slexer::basic_tokenstream<charT, idT> basic_lexer<charT, idT>::tokenize(const std::basic_string<charT> &_M_stream)
    {
        slexer::basic_tokenstream<charT, idT> _M_basic_tokenstream;
        master _M_master{_M_group_tables.size(), 80};
        typename std::basic_string<charT>::const_iterator it = _M_stream.begin();
        while (it < _M_stream.end())
        {
            handle _M_handle = nullptr;
            _M_master._capture._M_position = _M_capture_position;
            _M_master._capture._M_line = _M_capture_line;
            _M_master._capture._M_column = _M_capture_column;
            basic_lexer::_M_caption(it, _M_stream.end(), _M_master, _M_handle);
            if (_M_master.getflag(Flags::_S_emit_error))
            {
                if constexpr (std::is_same_v<charT, char>)
                    throw slexer::basic_lexical_error<charT, idT>("token inseperado o mal formado", _M_master._capture);
                else
                    throw slexer::basic_lexical_error<charT, idT>(L"token inseperado o mal formado", _M_master._capture);
            }
            if (_M_handle == nullptr)
                continue;
            _M_handle(_M_master);
            if (_M_master.getflag(Flags::_S_push_capture))
                _M_master._queue_emit.push(_M_master._capture);
            if (!_M_master.getflag(Flags::_S_emit_token))
                continue;
            while (!_M_master._queue_emit.empty())
            {
                _M_basic_tokenstream.push(std::move(_M_master._queue_emit.front()));
                _M_master._queue_emit.pop();
            }
        }
        return _M_basic_tokenstream;
    }
    template <typename idT>
    using lexer = basic_lexer<char, idT>;
    template <typename idT>
    using wlexer = basic_lexer<wchar_t, idT>;
} // namespace slexer
//<--...

#endif