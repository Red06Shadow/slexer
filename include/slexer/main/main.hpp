#ifndef SLEXERMAIN
#define SLEXERMAIN

#include <utilities/memory.hpp>
#include <myregex/automaton/table.hpp>
#include <slexer/stack/stack.hpp>
#include <slexer/token/token.hpp>
#include <slexer/tokenstream/tokenstream.hpp>
#include <slexer/exceptions/exceptions.hpp>
#include <utilities/selector.hpp>
//<--...
#include <iostream>
#include <fstream>
#include <filesystem>
#include <queue>

namespace slexer
{
    template <typename charT, typename idT>
    class basic_builder;

    template <typename charT, typename idT>
    class basic_lexer
    {
    public:
        static_assert(std::is_unsigned_v<idT> | std::is_enum_v<idT>, "in basic_lexer: do you no have template with value is not a unsigned integer");
        class master
        {
        private:
            /// @brief Contador de posicion de archivo
            inline void _M_manager_position(charT);

        private:
            /// @brief Inicio de tabla
            size_t _M_begin;
            /// @brief Tamano del grupo de tablas
            size_t _M_gsize;
            /// @brief Captura
            slexer::basic_token<charT, idT> _M_capture;
            /// @brief Queue
            std::queue<slexer::basic_token<charT, idT>> _M_queue;
            /// @brief Cadena de tokens
            slexer::basic_tokenstream<charT, idT> &_M_tokenstream;
            /// @brief Captura de posicion
            size_t _M_capture_position;
            /// @brief Captura de linea
            size_t _M_capture_line;
            /// @brief Captura de columna
            size_t _M_capture_column;

        public:
            /// @brief Pila de Buffers
            slexer::basic_stackbuffer<charT> stackbuffer;

        public:
            /// @brief Constructor por defecto
            /// @param _gsize Tamano del grupo de tablas
            /// @param _tokenstream Cadena de tokens
            /// @param _buffer Buffer
            master(size_t _gsize, slexer::basic_tokenstream<charT, idT> &_tokenstream, size_t size)
                : _M_gsize(_gsize), _M_begin(0), _M_tokenstream(_tokenstream), stackbuffer()
            {
                _M_capture._M_text.reserve(size);
            }
            /// @brief Restuarua el inicio a la tabla principal
            inline void begin() { _M_begin = 0; }
            /// @brief Cambia el inicio a la posicion de la tabla deseada
            /// @param _begin Posicion de inicio
            inline void begin(size_t _begin)
            {
                if (_begin >= _M_gsize)
                    throw slexer::critical_error(1, "no access to grupo over-size to table");
                _M_begin = _begin;
            }
            /// @brief Emite el token a la cadena de tokens
            inline void emit() { _M_tokenstream.push(_M_capture); }
            /// @brief Emite el token con un numero menor de caracteres a la cadena de tokens
            /// @param _less Numero de reduccion de caractres
            inline void emit(uint32_t _less)
            {
                if (_less > _M_capture.text().size())
                    _less = _M_capture.text().size();
                _M_capture.text().resize(_M_capture.text().size() - _less);
                stackbuffer.top().back(_less);
                _M_tokenstream.push(_M_capture);
            }
            /// @brief Cambia el token con una cadena modificada
            /// @param _str Cadena a insertar
            inline void change(const std::basic_string<char> &_str)
            {
                _M_capture._M_text = _str;
                _M_tokenstream.push(_M_capture);
            }
            /// @brief Cambia el token con una cadena e id modificada
            /// @param _str Cadena a insertar
            /// @param _id Id deseado
            inline void change(const std::basic_string<char> &_str, idT _id)
            {
                _M_capture._M_text = _str;
                _M_capture._M_id = _id;
                _M_tokenstream.push(_M_capture);
            }
            /// @brief Vacia la cola de capturas pendientes
            inline void pull()
            {
                while (!_M_queue.empty())
                {
                    _M_tokenstream.push(std::move(_M_queue.front()));
                    _M_queue.pop();
                }
            }
            /// @brief Vacia un numero especifico de capturas de la cola de pendientes
            /// @param _elements Cantidad de elementos a vaciar
            inline void pull(size_t _elements)
            {
                while (!_M_queue.empty() && _elements < _M_queue.size())
                {
                    _M_tokenstream.push(std::move(_M_queue.front()));
                    _M_queue.pop();
                }
            }
            /// @brief Obtiene la cola
            /// @return Cola de elementos de capturas
            inline std::queue<slexer::basic_token<charT, idT>> &queue() { return _M_queue; }
            /// @brief Obtiene la captura constante
            /// @return captura del lexer
            inline const slexer::basic_token<charT, idT> &capture() const { return _M_capture; }
            friend basic_lexer;
        };

        /// @brief Patron de la funcion de captura
        typedef void (*handle)(slexer::basic_lexer<charT, idT>::master &);

/// @brief Funcion de captura por defecto
#define defaultf(_charT, _idT) [](slexer::basic_lexer<_charT, _idT>::master &m) -> void { m.emit(); }
/// @brief Hanlde de definicion rapida
#define slexerf(_charT, _idT, _nmaster) [](slexer::basic_lexer<_charT, _idT>::master & _nmaster)->void
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
            friend std::ostream &operator<<(std::ostream &out, const _I_idT &element) { return out; }
            ~_I_idT() {}
        };

    private:
        /// @brief Tablas de expreseiones
        std::basic_allocator<myregex::basic_table<charT, _I_idT>> _M_group_tables;

    private: /// @brief Funciones privadass
        /// @brief Captura de token por fichero
        template <typename Funtion>
        bool _M_caption(master &, basic_lexer::handle &, Funtion);
        constexpr inline static size_t _S_value(charT caracter)
        {
            size_t value;
            if constexpr (std::is_same_v<charT, char>)
                return (uint8_t)(caracter);
            else
            {
                if constexpr (sizeof(wchar_t) == 2)
                    return (uint16_t)(caracter);
                else
                    return (uint32_t)(caracter);
            }
        }

    public:
        /// @brief Constructor base del lexer
        /// @param group Grupo de tablas de expresiones regulares
        /// @param size_buffer tamano del buffer
        basic_lexer(std::basic_allocator<myregex::basic_table<charT, _I_idT>> &&group)
            : _M_group_tables(std::move(group)) {}
        basic_lexer(const basic_lexer<charT, idT> &) = delete;

        /// @brief Constructor de movimiento base
        /// @param other otro objecto basic_lexer
        basic_lexer(basic_lexer<charT, idT> &&other)
            : _M_group_tables(std::move(other._M_group_tables)) {}

        basic_lexer<charT, idT> &operator=(const basic_lexer<charT, idT> &) = delete;
        basic_lexer<charT, idT> &operator=(basic_lexer<charT, idT> &&);
        slexer::basic_tokenstream<charT, idT> tokenize(std::basic_ifstream<charT> &_M_stream, size_t buffersize = 4096);
        slexer::basic_tokenstream<charT, idT> tokenize(const std::basic_string<charT> &str);
        size_t size() const
        {
            size_t size_ = 0;
            for (size_t i = 0; i < _M_group_tables.size(); i++)
                size_ = _M_group_tables[i].size();
            return size_;
        }

        void view()
        {
            for (size_t i = 0; i < _M_group_tables.size(); i++)
                std::selector<charT>::stream() << _M_group_tables[i];
        }

        void view(size_t i) { std::selector<charT>::stream() << _M_group_tables[i]; }

        friend basic_builder<charT, idT>;
    };

    template <typename charT, typename idT>
    basic_lexer<charT, idT> &basic_lexer<charT, idT>::operator=(basic_lexer<charT, idT> &&other)
    {
        if (this != &other)
            _M_group_tables = std::move(other._M_group_tables);
        return *this;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename charT, typename idT>
    void basic_lexer<charT, idT>::master::_M_manager_position(charT letter)
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
    template <typename Funtion>
    bool basic_lexer<charT, idT>::_M_caption(master &_M_master, basic_lexer<charT, idT>::handle &_M_handle, Funtion excecute)
    {
        size_t status = 0ULL;
        charT letter;
        std::basic_string<charT> str{};
        size_t max = 0ULL;
        size_t count = 0ULL;
        size_t accept_status = -1ULL;
        _I_idT reference;
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        if (_M_group_tables[_M_master._M_begin].status()[status].valid())
        {
            accept_status = status;
            reference = _M_group_tables[_M_master._M_begin].status()[status].get();
        }
        _M_master._M_capture._M_set_localete(
            _M_master._M_capture_line,
            _M_master._M_capture_column,
            _M_master._M_capture_position);
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        while (!_M_master.stackbuffer.top().end())
        {
            letter = _M_master.stackbuffer.top().peak();
            size_t next = _M_group_tables[_M_master._M_begin].transitions()[status * myregex::basic_table<charT, _I_idT>::dictionary + _S_value(letter)];
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            if (next == -1ULL)
                break; // No hay transición, rechazar
            status = next;
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            _M_master._M_manager_position(letter);
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            str.push_back(_M_master.stackbuffer.top().pop());
            count++;
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            if (_M_group_tables[_M_master._M_begin].status()[status].valid())
            {
                accept_status = status;
                reference = _M_group_tables[_M_master._M_begin].status()[status].get();
                max = str.size();
                count = 0ULL;
            }
            excecute();
        }
        if (count != 0)
            _M_master.stackbuffer.top().back(count);
        if (accept_status != -1ULL)
        {
            _M_master._M_capture._M_text = str.substr(0, max);
            _M_master._M_capture._M_id = reference.id();
            _M_handle = reference.funt();
        }
        return accept_status != -1ULL;
    }
    template <typename charT, typename idT>
    slexer::basic_tokenstream<charT, idT> basic_lexer<charT, idT>::tokenize(std::basic_ifstream<charT> &_M_stream, size_t buffersize)
    {
        slexer::basic_tokenstream<charT, idT> _M_basic_tokenstream;
        slexer::basic_lexer<charT, idT>::master _M_master = slexer::basic_lexer<charT, idT>::master(
            _M_group_tables.size(), 
            _M_basic_tokenstream, 
            buffersize);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _M_master.stackbuffer.push(buffersize);
        _M_stream >> _M_master.stackbuffer.top();
        auto v = []() -> void {};
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        while (!_M_master.stackbuffer.top().end())
        {
            handle _M_handle = nullptr;
            try
            {
                if (!basic_lexer::_M_caption(_M_master, _M_handle, [&_M_stream, &_M_master]() -> void
                                             {
            ///////////////////////////////////////////////////////////////////////////////////////////////////
                                                if (_M_master.stackbuffer.top().empty() && !_M_master.stackbuffer.top().eof())
                                                    _M_stream >> _M_master.stackbuffer.top(); 
                }))
                    throw slexer::basic_lexical_error<charT, idT>(0, "unesxpect token", _M_master._M_capture, default_hanlde_excpetion<charT, idT>);
                if (_M_handle == nullptr)
                    continue;
                _M_handle(_M_master);
            }
            catch (const slexer::basic_lexical_error<charT, idT> &e)
            {
                size_t before = _M_stream.tellg();
                std::cerr << e.what() << std::endl;
                std::selector<charT>::stream() << e.especification(_M_stream) << std::endl;
                _M_master.stackbuffer.top().pop();
                _M_stream.seekg(before);
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _M_master.stackbuffer.pop();
        return _M_basic_tokenstream;
    }

    template <typename charT, typename idT>
    slexer::basic_tokenstream<charT, idT> basic_lexer<charT, idT>::tokenize(const std::basic_string<charT> &str)
    {
        slexer::basic_tokenstream<charT, idT> _M_basic_tokenstream;
        slexer::basic_lexer<charT, idT>::master _M_master = slexer::basic_lexer<charT, idT>::master(
            _M_group_tables.size(), 
            _M_basic_tokenstream, 
            str.size());
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _M_master.stackbuffer.push(str);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        while (!_M_master.stackbuffer.top().end())
        {
            handle _M_handle = nullptr;
            try
            {
                if (!basic_lexer::_M_caption(_M_master, _M_handle, []() -> void {}))
                    throw slexer::basic_lexical_error<charT, idT>(0, "unesxpect token", _M_master._M_capture, default_hanlde_excpetion<charT, idT>);
                if (_M_handle == nullptr)
                    continue;
                _M_handle(_M_master);
            }
            catch (const slexer::basic_lexical_error<charT, idT> &e)
            {
                // std::cerr << e.what() << std::endl;
                // std::selector<charT>::stream() << e.especification(_M_stream) << std::endl;
                // _M_sbuffer_input.top().pop();
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _M_master.stackbuffer.pop();
        return _M_basic_tokenstream;
    }
    template <typename idT>
    using lexer = basic_lexer<char, idT>;
    template <typename idT>
    using wlexer = basic_lexer<wchar_t, idT>;
} // namespace slexer
//<--...

#endif