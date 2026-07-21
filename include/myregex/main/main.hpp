#ifndef MAINMREGEX
#define MAINMREGEX

#include <variant>

#include <myregex/automaton/nfa.hpp>
#include <myregex/automaton/dfa.hpp>
#include <myregex/automaton/table.hpp>
#include <utilities/range.hpp>
#include <utilities/selector.hpp>
#include <myregex/exceptions/error.hpp>

namespace myregex
{

    /// @brief Clase que almecena los resultados de match
    /// @tparam charT
    template <typename charT, typename idT>
    class caption
    {
        static_assert(std::is_same_v<charT, char> || std::is_same_v<charT, wchar_t>, "Error: no se permiten tipos de datos que no sean de caracteres(solo char o wchar_t)");

    private:
        typename std::__cxx11::basic_string<charT> _M_string;
        idT _M_id;

    public:
        caption() : _M_string({}), _M_id({}) {}
        caption(const typename std::__cxx11::basic_string<charT> &str, idT id) : _M_string(str), _M_id(id) {}
        caption(const charT *begin, const charT *end, idT id) : _M_string(begin, end), _M_id(id) {}
        inline const typename std::__cxx11::basic_string<charT> &str() const { return _M_string; }
        inline idT id() const { return _M_id; }
        ~caption() {}
    };

    /// @brief Almacena las constantes de myregex
    namespace constants
    {
        /// @brief Opciones de match
        enum class match_options : unsigned char
        {
            _S_first_sequence,
            _S_maximun_sequence,
            _S_default = _S_first_sequence
        };
        /// @brief Opciones de verificacion
        enum class verification_options : unsigned char
        {
        };
        /// @brief Opciones de busqueda
        enum class search_options : unsigned char
        {
            _S_first_coincidence,
            _S_multiple_coincidence,
            _S_default = _S_first_coincidence
        };
        /// @brief Opciones de remplazo
        enum class replace_options : unsigned char
        {
            _S_first_coincidence,
            _S_multiple_coincidence,
            _S_default = _S_first_coincidence
        };

        constexpr inline size_t caption_no_match = -1ULL;
        constexpr inline match_options match_first = match_options::_S_first_sequence;
        constexpr inline match_options match_maximun = match_options::_S_maximun_sequence;

        constexpr inline search_options search_first = search_options::_S_first_coincidence;
        constexpr inline search_options search_maximun = search_options::_S_multiple_coincidence;

        constexpr inline replace_options replace_first = replace_options::_S_first_coincidence;
        constexpr inline replace_options replace_maximun = replace_options::_S_multiple_coincidence;

    }

    template <typename charT, typename idT>
    class basic_regex
    {
    private:
        static_assert(std::is_same_v<charT, char> || std::is_same_v<charT, wchar_t>, "Error: no se permiten tipos de datos que no sean de caracteres(solo char o wchar_t)");
        std::variant<basic_nfa<charT, idT>, basic_dfa<charT, idT>, basic_table<charT, idT>> _M_expresions;

    public:
        /// @brief Constructor por defecto
        /// @param list
        basic_regex(std::variant<basic_nfa<charT, idT>, basic_dfa<charT, idT>, basic_table<charT, idT>> &&_vr) : _M_expresions(std::move(_vr)) {}
        static bool verification(const std::__cxx11::basic_string<charT> &str, const myregex::basic_nfa<charT, idT> &_nfa)
        {
            basic_string_range<charT> range = str;
            return myregex::basic_regex<charT, idT>::verification(range, _nfa);
        }
        static bool verification(const std::__cxx11::basic_string<charT> &str, const myregex::basic_dfa<charT, idT> &_dfa)
        {
            basic_string_range<charT> range = str;
            return myregex::basic_regex<charT, idT>::verification(range, _dfa);
        }
        static bool verification(const std::__cxx11::basic_string<charT> &str, const myregex::basic_table<charT, idT> &_table)
        {
            basic_string_range<charT> range = str;
            return myregex::basic_regex<charT, idT>::verification(range, _table);
        }
        static bool verification(basic_string_range<charT> &range, const myregex::basic_nfa<charT, idT> &_nfa);
        static bool verification(basic_string_range<charT> &range, const myregex::basic_dfa<charT, idT> &_dfa);
        static bool verification(basic_string_range<charT> &range, const myregex::basic_table<charT, idT> &_table);

        /// @brief Esta funcion verifica si la cadena representada por str es valida
        /// @param str Cadena a leer
        /// @return Verdarero si lo es falso si no
        inline bool verification(const std::__cxx11::basic_string<charT> &str) const; // { /*return myregex::basic_regex<charT, idT>::verification(str, );*/ }
        /// @brief Esta funcion verifica si la cadena representada por range es valida
        /// @param str rango de caracteres de una cadena (para lexers)
        /// @return Verdarero si lo es falso si no
        inline bool verification(basic_string_range<charT> &range) const;

        template <myregex::constants::match_options option>
        static myregex::caption<charT, idT> match(const std::__cxx11::basic_string<charT> &str, const myregex::basic_nfa<charT, idT> &_nfa)
        {
            basic_string_range<charT> range = str;
            return myregex::basic_regex<charT, idT>::template match<option>(range, _nfa);
        }
        template <myregex::constants::match_options option>
        static myregex::caption<charT, idT> match(const std::__cxx11::basic_string<charT> &str, const myregex::basic_dfa<charT, idT> &_dfa)
        {
            basic_string_range<charT> range = str;
            return myregex::basic_regex<charT, idT>::template match<option>(range, _dfa);
        }
        template <myregex::constants::match_options option>
        static myregex::caption<charT, idT> match(const std::__cxx11::basic_string<charT> &str, const myregex::basic_table<charT, idT> &_table)
        {
            basic_string_range<charT> range = str;
            return myregex::basic_regex<charT, idT>::template match<option>(range, _table);
        }

        template <myregex::constants::match_options option>
        static myregex::caption<charT, idT> match(basic_string_range<charT> &range, const myregex::basic_nfa<charT, idT> &_nfa);
        template <myregex::constants::match_options option>
        static myregex::caption<charT, idT> match(basic_string_range<charT> &range, const myregex::basic_dfa<charT, idT> &_dfa);
        template <myregex::constants::match_options option>
        static myregex::caption<charT, idT> match(basic_string_range<charT> &range, const myregex::basic_table<charT, idT> &_table);

        /// @brief Esta funcion capta o valida segun el parametro S las cadenas de caracteres dentro de str
        /// @tparam S Determina la operacion de la funcion
        /// @param str Cadena a leer
        /// @return Devuelve el iterador al final de la cadena captada o nullptr si hay error junto al identificador de estado de aceptacion
        template <myregex::constants::match_options option>
        inline myregex::caption<charT, idT> match(const std::__cxx11::basic_string<charT> &str) const;
        /// @brief Esta funcion capta o valida segun el parametro S las cadenas de caracteres dentro del range
        /// @tparam S Determina la operacion de la funcion
        /// @param str rango de caracteres de una cadena (para lexers)
        /// @return Devuelve el iterador al final de la cadena captada o nullptr si hay error junto al identificador de estado de aceptacion
        template <myregex::constants::match_options option>
        inline myregex::caption<charT, idT> match(basic_string_range<charT> &range) const;

        inline size_t size() const;
        inline void export_automaton(std::basic_ostream<charT>& out) const;
        inline void view() const { export_automaton(std::selector<charT>::stream()); }
    };

    template <typename idT>
    using CompatibleRegex = myregex::basic_regex<char, idT>;
    template <typename idT>
    using regex = myregex::basic_regex<char, idT>;
    /////////////////////////////////////////////////////////////////////
    template <typename idT>
    using UnicodeRegex = myregex::basic_regex<wchar_t, idT>;
    template <typename idT>
    using wregex = myregex::basic_regex<wchar_t, idT>;

    template <typename charT, typename idT>
    bool basic_regex<charT, idT>::verification(basic_string_range<charT> &range, const myregex::basic_nfa<charT, idT> &_nfa)
    {
        std::set<size_t> status = myregex::basic_builder<charT, idT>::_S_elipson_cloursers({0}, _nfa);
        while (range.peak() < range.end())
        {
            charT letter = *range.peak();
            status = myregex::basic_builder<charT, idT>::_S_move_elipson_cloursers(status, letter, _nfa);
            if (status.empty())
                break;
            range.next();
        }
        // for (auto &&state : status)
        //     if (typename myregex::basic_nfa<char, idT>::Fnfa::const_iterator it = _nfa.accepted_status().find(state); it != _nfa.accepted_status().end())
        //         return true;
        for (auto &&state : status)
            if (_nfa.status()[state].valid())
                return true;
        return false;
    }

    template <typename charT, typename idT>
    bool basic_regex<charT, idT>::verification(basic_string_range<charT> &range, const myregex::basic_dfa<charT, idT> &_dfa)
    {
        size_t status = 0;
        while (range.peak() < range.end())
        {
            charT letter = *range.peak();
            auto transition = _dfa.transitions().find({status, letter});
            if (transition == _dfa.transitions().end())
                return false;
            status = transition->second;
            range.next();
        }
        return range.peak() == range.end() && _dfa.status()[status].valid();
    }
    template <typename charT, typename idT>
    bool basic_regex<charT, idT>::verification(basic_string_range<charT> &range, const myregex::basic_table<charT, idT> &_table)
    {
        size_t status = 0;
        while (range.peak() < range.end())
        {
            charT letter = *range.peak();
            if ((status = _table.transitions()[(status * _table.dictionary) + letter]) == -1ULL)
                return false; // No hay transición, rechazar
            range.next();
        }
        return range.peak() == range.end() && _table.status()[status].valid();
    }

    template <typename charT, typename idT>
    bool basic_regex<charT, idT>::verification(const std::__cxx11::basic_string<charT> &str) const
    {
        return std::visit([&str](auto &&value) -> bool
                          {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, basic_nfa<charT, idT>> || std::is_same_v<type, basic_dfa<charT, idT>> || std::is_same_v<type, basic_table<charT, idT>>)
                return myregex::basic_regex<charT, idT>::verification(str, value);
            else
                return false; }, _M_expresions);
    }
    template <typename charT, typename idT>
    bool basic_regex<charT, idT>::verification(basic_string_range<charT> &range) const
    {
        return std::visit([&range](auto &&value) -> bool
                          {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, basic_nfa<charT, idT>> || std::is_same_v<type, basic_dfa<charT, idT>> || std::is_same_v<type, basic_table<charT, idT>>)
                return myregex::basic_regex<charT, idT>::verification(range, value);
            else
                return false; }, _M_expresions);
    }

    template <typename charT, typename idT>
    template <myregex::constants::match_options option>
    myregex::caption<charT, idT> basic_regex<charT, idT>::match(basic_string_range<charT> &range, const myregex::basic_nfa<charT, idT> &_nfa)
    {
        std::__cxx11::basic_string<charT> _M_string{};
        idT id{};
        bool _M_valid = false;
        std::set<size_t> status = myregex::basic_builder<charT, idT>::_S_elipson_cloursers({0}, _nfa);
        while (range.peak() < range.end())
        {
            idT __id{};
            charT letter = *range.peak();
            status = myregex::basic_builder<charT, idT>::_S_move_elipson_cloursers(status, letter, _nfa);
            if (status.empty())
                break;
            _M_string.push_back(letter);
            for (auto &&state : status)
            {
                if (_nfa.status()[state].valid() && (!_M_valid || _nfa.status()[state].get() < __id))
                {
                    __id = _nfa.status()[state].get();
                    _M_valid = true;
                }
            }
            if constexpr (option == myregex::constants::match_options::_S_first_sequence)
                return {_M_string, __id};
            else if constexpr (option == myregex::constants::match_options::_S_maximun_sequence)
                id = __id;
            range.next();
        }
        if constexpr (option == myregex::constants::match_options::_S_maximun_sequence)
            return myregex::caption<charT, idT>(_M_string, id);
        else if constexpr (option == myregex::constants::match_options::_S_first_sequence)
            return myregex::caption<charT, idT>({}, {});
    }

    template <typename charT, typename idT>
    template <myregex::constants::match_options option>
    myregex::caption<charT, idT> basic_regex<charT, idT>::match(basic_string_range<charT> &range, const myregex::basic_dfa<charT, idT> &_dfa)
    {
        std::__cxx11::basic_string<charT> _M_string{};
        idT id{};
        size_t status = 0;
        size_t acceptance_status = -1ULL;
        while (range.peak() < range.end())
        {
            charT letter = *range.peak();
            auto transition = _dfa.transitions().find({status, letter});
            if (transition == _dfa.transitions().end())
                break; // No hay transición, rechazar
            status = transition->second;
            _M_string.push_back(letter);
            if (status == acceptance_status)
            {
                range.next();
                continue;
            }
            else if (_dfa.status()[status].valid())
            {
                acceptance_status = status;
                if constexpr (option == myregex::constants::match_options::_S_first_sequence)
                    return {_M_string, _dfa.accepted_status().at(acceptance_status)};
                else if constexpr (option == myregex::constants::match_options::_S_maximun_sequence)
                    id = _dfa.status()[acceptance_status].get();
            }
            range.next();
        }
        if constexpr (option == myregex::constants::match_options::_S_maximun_sequence)
            return myregex::caption<charT, idT>(_M_string, id);
        else if constexpr (option == myregex::constants::match_options::_S_first_sequence)
            return myregex::caption<charT, idT>({}, {});
    }
    template <typename charT, typename idT>
    template <myregex::constants::match_options option>
    myregex::caption<charT, idT> basic_regex<charT, idT>::match(basic_string_range<charT> &range, const myregex::basic_table<charT, idT> &_table)
    {
        std::__cxx11::basic_string<charT> _M_string{};
        idT id{};
        size_t status = 0;
        size_t acceptance_status = -1ULL;
        while (range.peak() < range.end())
        {
            charT letter = *range.peak();
            size_t next_state = _table.transitions()[(status * _table.dictionary) + myregex::basic_builder<charT, idT>::_S_transition(letter)];
            if (next_state == -1ULL)
                break; // No hay transición, rechazar
            status = next_state;
            _M_string.push_back(letter);
            if (status == acceptance_status)
            {
                range.next();
                continue;
            }
            else if (_table.status()[status].valid())
            {
                acceptance_status = status;
                if constexpr (option == myregex::constants::match_options::_S_first_sequence)
                    return {_M_string, _table.accepted_status().at(acceptance_status)};
                else if constexpr (option == myregex::constants::match_options::_S_maximun_sequence)
                    id = _table.status()[acceptance_status].get();
            }
            range.next();
        }
        if constexpr (option == myregex::constants::match_options::_S_maximun_sequence)
            return myregex::caption<charT, idT>(_M_string, id);
        else if constexpr (option == myregex::constants::match_options::_S_first_sequence)
            return myregex::caption<charT, idT>({}, {});
    }

    template <typename charT, typename idT>
    template <myregex::constants::match_options option>
    myregex::caption<charT, idT> basic_regex<charT, idT>::match(const std::__cxx11::basic_string<charT> &str) const
    {
        return std::visit([&str](auto &&value) -> myregex::caption<charT, idT>
                          {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, basic_nfa<charT, idT>> || std::is_same_v<type, basic_dfa<charT, idT>> || std::is_same_v<type, basic_table<charT, idT>>)
                return myregex::basic_regex<charT, idT>::template match<option>(str, value);
            else
                return {}; }, _M_expresions);
    }
    template <typename charT, typename idT>
    template <myregex::constants::match_options option>
    myregex::caption<charT, idT> basic_regex<charT, idT>::match(basic_string_range<charT> &range) const
    {
        return std::visit([&range](auto &&value) -> myregex::caption<charT, idT>
                          {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, basic_nfa<charT, idT>> || std::is_same_v<type, basic_dfa<charT, idT>> || std::is_same_v<type, basic_table<charT, idT>>)
                return myregex::basic_regex<charT, idT>::template match<option>(range, value);
            else
                return false; }, _M_expresions);
    }

    template <typename charT, typename idT>
    size_t basic_regex<charT, idT>::size() const
    {
        return std::visit([](auto &&value) -> size_t
                          {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, basic_nfa<charT, idT>> || std::is_same_v<type, basic_dfa<charT, idT>> || std::is_same_v<type, basic_table<charT, idT>>)
                return value.size();
            else
                return 0; }, _M_expresions);
    }
    template <typename charT, typename idT>
    void basic_regex<charT, idT>::export_automaton(std::basic_ostream<charT>& out) const {
        std::visit([&out](auto &&value) -> void
                   {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, basic_nfa<charT, idT>> || std::is_same_v<type, basic_dfa<charT, idT>> || std::is_same_v<type, basic_table<charT, idT>>)
                out << value; 
            }, _M_expresions);
    }
} // namespace myregex

#endif