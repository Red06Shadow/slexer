#ifndef BUILDMREGEX
#define BUILDMREGEX

#include <myregex/main/main.hpp>
#include <stack>
#include <queue>
#include <unordered_set>
#include <math.h>

namespace myregex
{
    template <typename charT, typename idT>
    class basic_builder
    {
        static_assert(std::is_same_v<charT, char> || std::is_same_v<charT, wchar_t>, "Error: no se permiten tipos de datos que no sean de caracteres(solo char o wchar_t)");

    private: // enums, structs
        using _M_begin_transitions = std::pair<size_t, std::vector<size_t>>;
        using _M_range_cualifiquer = std::pair<size_t, size_t>;

    private: // Funtions regex -> myregex::basic_nfa
        // Funciones para el parseo de expresiones regex a myregex::basic_nfa
        static charT _S_parser_code_caracter_hexunicode(basic_string_range<charT> &, const unsigned char);
        static charT _S_parser_code_caracter_octal(basic_string_range<charT> &);
        static charT _S_parser_code_caracter(basic_string_range<charT> &);
        static _M_range_cualifiquer _S_parser_cualifiquer_range(basic_string_range<charT> &);
        static void _S_parser_cualifiquer(basic_string_range<charT> &, myregex::basic_nfa<charT, idT> &, const _M_begin_transitions &, bool);
        static _M_begin_transitions _S_parser_nfa_parser_class_expresions(basic_string_range<charT> &, myregex::basic_nfa<charT, idT> &);
        static size_t _S_build_nfa_parser_regular_expresions_basic(basic_string_range<charT> &, myregex::basic_nfa<charT, idT> &, bool, idT);
        static size_t _S_build_nfa_parser_or_expresions(basic_string_range<charT> &, myregex::basic_nfa<charT, idT> &, bool, idT);

        constexpr inline static size_t _S_transition(charT caracter)
        {
            size_t transition;
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

    public: // Funtions myregex::basic_nfa -> basic_dfa
        // Funciones adicionales del convertidor de myregex::basic_nfa a basic_dfa
        static std::set<size_t> _S_elipson_cloursers(const std::set<size_t> &, const myregex::basic_nfa<charT, idT> &);
        static std::set<size_t> _S_move(const std::set<size_t> &, charT, const myregex::basic_nfa<charT, idT> &);
        inline static std::set<size_t> _S_move_elipson_cloursers(const std::set<size_t> &states, charT a, const myregex::basic_nfa<charT, idT> &nfa) { return _S_elipson_cloursers(_S_move(states, a, nfa), nfa); }

    private:
        std::variant<basic_nfa<charT, idT>, basic_dfa<charT, idT>, basic_table<charT, idT>> _M_expresions;

    public:
        basic_builder(const std::initializer_list<std::pair<idT, std::__cxx11::basic_string<charT>>> &list) { _M_expresions = std::move(build_nfa(list)); }
        basic_builder(const std::vector<std::pair<idT, std::__cxx11::basic_string<charT>>> &list) { _M_expresions = std::move(build_nfa(list)); }
        static myregex::basic_nfa<charT, idT> build_nfa(const std::initializer_list<std::pair<idT, std::__cxx11::basic_string<charT>>> &list);
        static myregex::basic_nfa<charT, idT> build_nfa(const std::vector<std::pair<idT, std::__cxx11::basic_string<charT>>> &list);
        static myregex::basic_dfa<charT, idT> build_dfa(const basic_nfa<charT, idT> &nfa);
        static myregex::basic_table<charT, idT> build_table(const basic_dfa<charT, idT> &dfa);
        myregex::basic_builder<charT, idT> &convert_to_dfa()
        {
            myregex::basic_dfa<charT, idT> _M_provitional = std::visit([](auto &&value) -> myregex::basic_dfa<charT, idT>
                                                                       {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, basic_nfa<charT, idT>>)
                return build_dfa(value);
            else
                throw myregex::basic_regex_error<charT>("imposible operation whit not nfa struct", myregex::error_type::_S_runtime_error, 20); }, _M_expresions);
            _M_expresions = _M_provitional;
            return *this;
        }
        myregex::basic_builder<charT, idT> &convert_to_table()
        {
            myregex::basic_table<charT, idT> _M_provitional = std::visit([](auto &&value) -> myregex::basic_table<charT, idT>
                                                                         {
            using type = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<type, basic_dfa<charT, idT>>)
                return build_table(value);
            else
                throw myregex::basic_regex_error<charT>("imposible operation whit not dfa struct", myregex::error_type::_S_runtime_error, 21); }, _M_expresions);
            _M_expresions = _M_provitional;
            return *this;
        }
        myregex::basic_regex<charT, idT> build() { return std::move(_M_expresions); }
        friend basic_regex<charT, idT>;
    };

    template <typename idT>
    using CompatibleBuilder = myregex::basic_builder<char, idT>;
    template <typename idT>
    using builder = myregex::basic_builder<char, idT>;
    /////////////////////////////////////////////////////////////////////
    template <typename idT>
    using UnicodeBuilder = myregex::basic_builder<wchar_t, idT>;
    template <typename idT>
    using wbuilder = myregex::basic_builder<wchar_t, idT>;

    template <typename charT, typename idT>
    charT myregex::basic_builder<charT, idT>::_S_parser_code_caracter_hexunicode(basic_string_range<charT> &range, const unsigned char _S)
    {
        charT result;
        for (charT i = 0; i < _S; i++)
        {
            if (range.peak() >= range.end())
                throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);
            charT caracter = *range.peak();
            if (caracter >= charT('a') && caracter <= charT('f'))
                result = result * 16 + (caracter - charT('a') + 10);
            else if (caracter >= charT('A') && caracter <= charT('F'))
                result = result * 16 + (caracter - charT('A') + 10);
            else if (caracter >= charT('0') && caracter <= charT('9'))
                result = result * 16 + (caracter - charT('0'));
            else
                throw myregex::basic_regex_error<charT>("caracter no permitido en el formato de caracter unicode", range.position(range.peak()), range, 1);
            range.next();
        }
        return result;
    }

    template <typename charT, typename idT>
    charT myregex::basic_builder<charT, idT>::_S_parser_code_caracter_octal(basic_string_range<charT> &range)
    {
        char result;
        for (char i = 0; i < 3; i++)
        {
            if (range.peak() >= range.end() && i < 1)
                throw myregex::basic_regex_error<charT>("formato de caracter octal incorrecto", range.position(range.end(), 1ULL), range, 2);
            char caracter = *range.peak();
            if (caracter >= '0' && caracter <= '7')
                result = result * 8 + (caracter - '0');
            else
                break;
            range.next();
        }
        return result;
    }

    template <typename charT, typename idT>
    charT myregex::basic_builder<charT, idT>::_S_parser_code_caracter(basic_string_range<charT> &range)
    {
        charT c;
        if (range.peak() >= range.end())
            throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);
        switch (c = *range.peak())
        {
        case charT('n'):
            range.next();
            return charT('\n');
            break;
        case charT('t'):
            range.next();
            return charT('\t');
            break;
        case charT('v'):
            range.next();
            return charT('\v');
            break;
        case charT('f'):
            range.next();
            return charT('\f');
            break;
        case charT('b'):
            range.next();
            return charT('\b');
            break;
        case charT('r'):
            range.next();
            return charT('\r');
            break;
        case charT('x'):
            range.next();
            return basic_builder<charT, idT>::_S_parser_code_caracter_hexunicode(range, 2);
            break;
        case charT('u'):
            range.next();
            return basic_builder<charT, idT>::_S_parser_code_caracter_hexunicode(range, 4);
            break;
        case charT('U'):
            range.next();
            return basic_builder<charT, idT>::_S_parser_code_caracter_hexunicode(range, 8);
            break;
        default:
            if ('0' <= c && c <= '7')
                return basic_builder<charT, idT>::_S_parser_code_caracter_octal(range);
            range.next();
            return c;
            break;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////Parser////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////

    template <typename charT, typename idT>
    typename myregex::basic_builder<charT, idT>::_M_range_cualifiquer myregex::basic_builder<charT, idT>::_S_parser_cualifiquer_range(basic_string_range<charT> &range)
    {
        size_t n1 = 0;    // minimo
        size_t n2 = 0;    // maximo
        size_t *rn = &n1; // puntero al numero actual
        charT caracter;
        bool invalid = true; // Determina si es valido el resultado
        size_t position_start = range.position(range.peak(), -1ULL);
        // Recorremos la cadena de caracteres
        while (range.peak() < range.end() and (caracter = *range.peak()) != '}')
        {
            // Si es un numero
            if (charT('0') <= caracter && caracter <= charT('9'))
            {
                // Realizamos los calculos para la conversion
                *rn = *rn * 10 + caracter - charT('0');
                // Y desactivamos la invalidacion
                invalid = false;
            }
            else if (charT(',') == caracter) // Si es una coma
            {
                // Y el rn apunta a el numero 2(maximo)
                if (invalid)
                    throw myregex::basic_regex_error<charT>("min value no inserted", range.position(range.peak()), range, 16);
                if (rn == &n2)
                    throw myregex::basic_regex_error<charT>("invalid subdivision", range.position(range.peak()), range, 4);
                // Cambiamos el puntero de n1(minimo) a n2(maximo)
                rn = &n2;
                // Y activamos la invalidacion
                invalid = true;
            }
            else // Si no solo se emite el error de caracter invalido
                throw myregex::basic_regex_error<charT>("invalid caracter inserted", range.position(range.peak()), range, 5);
            range.next(); // Y por ultimo pasamos al siguiente
        }
        if (range.peak() == range.end()) // Si no se llego al final del cualificador, emitir error
            throw myregex::basic_regex_error<charT>("unespect termination of range", range.position(range.end(), 1ULL), range, 6);
        if (invalid) // si es invalida, emitir tambien un error
            throw myregex::basic_regex_error<charT>("invalid format inserted", position_start, range.position(range.peak()), range, 7);
        if (rn == &n1) // si el puntero permanece en n1(minimo), significa que el rango es {n1, n1} minimo igual a maximo
            n2 = n1;
        if (n2 < n1)
            throw myregex::basic_regex_error<charT>("range whit min greather than max in this cualifiquer", position_start, range.position(range.peak()), range, 14);
        // Retornar el par de numeros
        return {n1, n2};
    }

    template <typename charT, typename idT>
    void basic_builder<charT, idT>::_S_parser_cualifiquer(basic_string_range<charT> &range, basic_nfa<charT, idT> &nfa, const basic_builder<charT, idT>::_M_begin_transitions &_T_qAtr, bool isGroup)
    {
        charT c = 0;
        // Si el rango llega al final salta a la sentemcia por defecto del switch
        if (range.peak() >= range.end())
            goto labelForEnd;
        // Si no obtenemos el caracter en esa posicion
        c = *range.peak();
        switch (c)
        {
        case charT('?'):
        {
            // Si se cumple se agrega la opcionalidad al estado
            range.next();
            // Primera se agregan las transiciones generadas siempre y cuando no sean un grupo
            if (!isGroup)
                for (auto &&transition : _T_qAtr.second)
                    nfa.Q_transitions[{_T_qAtr.first, transition}].push_back(nfa.Q_nfa.size());
            // luego se agrega la trasicion de la opcionalidad
            nfa.Q_transitions[{_T_qAtr.first, -1ULL}].push_back(nfa.Q_nfa.size());
            break;
        }
        case charT('*'):
        {
            // Si se cumple se agrega la repeticion de cero a multiples veces
            range.next();
            // Si es un grupo las transiciones de repeticion se realizan desde el final hasta el principio
            if (isGroup)
                for (auto &&transition : _T_qAtr.second)
                    nfa.Q_transitions[{nfa.Q_nfa.size(), transition}].push_back(_T_qAtr.first);
            else // si no simplemente se agregan al mismo estado
                for (auto &&transition : _T_qAtr.second)
                    nfa.Q_transitions[{_T_qAtr.first, transition}].push_back(_T_qAtr.first);
            nfa.Q_transitions[{_T_qAtr.first, -1ULL}].push_back(nfa.Q_nfa.size());
            break;
        }
        case charT('+'):
        {
            // Si se cumple se agrega la repeticion de cero a multiples veces
            range.next();
            if (!isGroup)
                for (auto &&transition : _T_qAtr.second)
                    nfa.Q_transitions[{_T_qAtr.first, transition}].push_back(nfa.Q_nfa.size());
            nfa.Q_transitions[{nfa.Q_nfa.size(), -1ULL}].push_back(_T_qAtr.first);
            break;
        }
        case charT('{'):
        {
            range.next();
            _M_range_cualifiquer cualifiquer_range = basic_builder<charT, idT>::_S_parser_cualifiquer_range(range);
            range.next();
            if (isGroup)
            {
                size_t n_state = nfa.Q_nfa.size() - _T_qAtr.first;
                std::vector<size_t> optionals_groups = {};
                if (cualifiquer_range.first == 0)
                    optionals_groups.push_back(_T_qAtr.first);
                for (size_t state = 1; state < cualifiquer_range.second; state++)
                {
                    size_t state_group_begin = nfa.Q_nfa.size();
                    if (cualifiquer_range.first <= state && state < cualifiquer_range.second)
                        // Agregar una trasicion de opcionalidad
                        optionals_groups.push_back(state_group_begin);
                    for (size_t state_group = 0; state_group < n_state; state_group++)
                    {
                        nfa.Q_nfa.push_back(nfa.Q_nfa[state_group_begin + state_group]);
                        for (auto &&[key, value] : nfa.Q_transitions)
                        {
                            if (key.first != _T_qAtr.first + state_group)
                                continue;
                            nfa.Q_transitions[{state_group_begin + state_group, key.second}].push_back(nfa.Q_nfa.size());
                        }
                    }
                }
                for (auto &&state : optionals_groups)
                    nfa.Q_transitions[{state, -1ULL}].push_back(nfa.Q_nfa.size());
            }
            else // si no simplemente se agregan al mismo estado
            {
                size_t state_iterator = _T_qAtr.first;
                for (size_t state = 0; state < cualifiquer_range.second; state++)
                {
                    // Si el estado actual esta entre el minimo y el maximo
                    if (cualifiquer_range.first <= state && state < cualifiquer_range.second)
                        // Agregar una trasicion de opcionalidad
                        nfa.Q_transitions[{state_iterator, -1ULL}].push_back(_T_qAtr.first + cualifiquer_range.second);
                    // Y luego se agregan todas las transiciones desde el estado actual hasta el proximo estado
                    for (auto &&transition : _T_qAtr.second)
                        nfa.Q_transitions[{state_iterator, transition}].push_back(nfa.Q_nfa.size());
                    // por ultimo se guarda el nuevo estado
                    state_iterator = nfa.Q_nfa.size();
                    // Y si el rango es menor que el maximo - 1 se agrega el nuevo estado generado
                    if (state < cualifiquer_range.second - 1ULL)
                        nfa.Q_nfa.push_back(nfa.Q_nfa[state]);
                }
            }
            break;
        }
        default:
        labelForEnd:
            // Por ultimo en caso de no cumplirse los casos anteriores o la condicion de final de cadena
            if (isGroup) // si es un grupo, no se agregan dichas transiciones
                break;
            // Pero si no, se agregan las transiciones del estado
            for (auto &&transition : _T_qAtr.second)
                nfa.Q_transitions[{_T_qAtr.first, transition}].push_back(nfa.Q_nfa.size());
            break;
        }
    }

    template <typename charT, typename idT>
    typename basic_builder<charT, idT>::_M_begin_transitions basic_builder<charT, idT>::_S_parser_nfa_parser_class_expresions(basic_string_range<charT> &range, basic_nfa<charT, idT> &nfa)
    {
        size_t qA = nfa.Q_nfa.size();
        std::vector<size_t> transitions = {};
        std::unordered_set<charT> alphabet = {};
        bool inverter = false;
        size_t position_start = range.position(range.peak(), -1ULL);

        nfa.Q_nfa.push_back({});
        // Chequeamos si es una clase invertida
        if (*range.peak() == charT('^'))
        {
            // Si lo es hacer operaciones de inversion, por ahora no se ha implementado
            inverter = true;
            range.next();
        }
        charT a;
        // Recorremos la cadena hasta llegar el final
        while (range.peak() < range.end() and (a = *range.peak()) != charT(']'))
        {
            // Pasamos al siguiente valor
            size_t position_start_range = range.position(range.peak());
            range.next();

            if (a == charT('-'))
                throw myregex::basic_regex_error<charT>("invalid caracter in class with no initialice range whit caracter", range.position(range.peak(), -1ULL), range, 0);

            if (a == charT('\\'))                                              // Si es un caracter de escape
                a = basic_builder<charT, idT>::_S_parser_code_caracter(range); // Ejecutar caracteres de escape

            // Si el iterador llego a su fin
            if (range.peak() >= range.end())
                throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);

            // Si el caracter proximo no es un caracter '-'
            if (*(range.peak()) != charT('-'))
                alphabet.insert(a); // Solamente insertar el caracter actual
            else
            {
                // En caso contrario obtenemos el rango de transiciones
                auto offset = range.offset(1);
                char b;
                if (offset >= range.end())
                    throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);
                if ((b = *offset) == charT(']'))
                    throw myregex::basic_regex_error<charT>("termination max incurret", range.position(offset), range, 9);
                range.next();
                range.next();
                if (b == charT('\\'))
                    b = basic_builder<charT, idT>::_S_parser_code_caracter(range);
                if (b <= a)
                    throw myregex::basic_regex_error<charT>("range whit min greather than max in this class", position_start_range, range.position(offset), range, 10);
                for (size_t i = a; i <= b; i++)
                    alphabet.insert(charT(i));
            }
        }
        // Si la clase esta invertida se agregan todos los caracteres que no fueron detectados dentro del su definicion
        if (alphabet.empty())
            throw myregex::basic_regex_error<charT>("this class is empty", position_start, range.position(range.peak()), range, 8);
        if (inverter)
        {
            // Se recorre todos los caracteres
            for (size_t i = 0; i < size_t(std::pow(256, sizeof(charT))); i++)
            {
                // Si el caracter no se encuentra dentro del conjunto
                if (alphabet.count(charT(i)) < 1)
                {
                    // Se agrega la transicion y luego el caracter al alfabeto
                    transitions.push_back(myregex::basic_builder<charT, idT>::_S_transition(i));
                    nfa.Q_dictionary.insert(charT(i));
                }
            }
        }
        else
        {
            // Si no solo se agregan las transiciones y caracteres al alfabeto definidos en la clase
            for (auto &&i : alphabet)
            {
                transitions.push_back(myregex::basic_builder<charT, idT>::_S_transition(i));
                nfa.Q_dictionary.insert(i);
            }
        }
        // Retorna el estado inicial y las transiciones
        return {qA, transitions};
    }

    template <typename charT, typename idT>
    size_t basic_builder<charT, idT>::_S_build_nfa_parser_regular_expresions_basic(basic_string_range<charT> &range, basic_nfa<charT, idT> &nfa, bool isGroup, idT id)
    {
        // Dado el rango de caracteres
        while (range.peak() < range.end())
        {
            // Obtenemos el caracter actual
            basic_builder<charT, idT>::_M_begin_transitions _T_qAtr;
            charT c = *range.peak();
            if (c == charT('|') or c == charT(')'))
                break;
            // Y pasamos al siguiente
            range.next();
            // Segun el caracter que sea
            switch (c)
            {
            case charT('('):
            {
                size_t position_start = range.position(range.peak(), -1ULL);
                // Genera un grupo de reglas, se utiliza la recursion de la fincion recursiva
                _T_qAtr = {basic_builder<charT, idT>::_S_build_nfa_parser_or_expresions(range, nfa, true, id), {-1ULL}};
                // Si no termina en la posicion correcta, error
                if (range.peak() >= range.end())
                    throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);
                if (*range.peak() != charT(')'))
                    throw myregex::basic_regex_error<charT>("bad termination in group", range.position(range.peak()), range, 11);
                // Saltamos el final del grupo
                range.next();
                // Y agregamos su respectivo cualificador
                basic_builder<charT, idT>::_S_parser_cualifiquer(range, nfa, _T_qAtr, true);
                break;
            }
            case charT('['):
            {
                // Genera un estado para clases de caracteres
                _T_qAtr = basic_builder<charT, idT>::_S_parser_nfa_parser_class_expresions(range, nfa);
                // Si no termina en la posicion correcta, error
                if (range.peak() >= range.end())
                    throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);
                if (*range.peak() != charT(']'))
                    throw myregex::basic_regex_error<charT>("bad termination in class", range.position(range.peak()), range, 12);
                // Saltamos el final del grupo
                range.next();
                // Y agregamos su respectivo cualificador
                basic_builder<charT, idT>::_S_parser_cualifiquer(range, nfa, _T_qAtr, false);
                break;
            }
            // case charT('^'):
            // {
            //     if (nfa.Q_nfa.back().questbeginline)
            //         throw myregex::basic_regex_error<charT>("do you has mention this status when begin line", range.position(range.peak(), -1ULL), range, 17);
            //     nfa.Q_nfa.back().questbeginline = true;
            //     break;
            // }
            // case charT('$'):
            // {
            //     if (nfa.Q_nfa.back().questendline)
            //         throw myregex::basic_regex_error<charT>("do you has mention this status when end line", range.position(range.peak(), -1ULL), range, 18);
            //     nfa.Q_nfa.back().questendline = true;
            //     break;
            // }
            case charT('*'):
            case charT('+'):
            case charT('?'):
            case charT('{'):
                throw myregex::basic_regex_error<charT>("cualifiquer unespect in this operation", range.position(range.peak(), -1ULL), range, 15);
            case charT('\\'):
                c = basic_builder<charT, idT>::_S_parser_code_caracter(range);
            default:
            {
                // Simplemente creamos el estado y agregamos la trsnicion
                size_t qA = nfa.Q_nfa.size();
                nfa.Q_dictionary.insert(c);
                nfa.Q_nfa.push_back({});
                // Y agregamos su respectivo cualificador
                basic_builder<charT, idT>::_S_parser_cualifiquer(range, nfa, {qA, {myregex::basic_builder<charT, idT>::_S_transition(c)}}, false);
                break;
            }
            }
        }
        size_t qEnd = nfa.Q_nfa.size();
        nfa.Q_nfa.push_back({});
        return qEnd;
    }

    template <typename charT, typename idT>
    size_t basic_builder<charT, idT>::_S_build_nfa_parser_or_expresions(basic_string_range<charT> &range, basic_nfa<charT, idT> &nfa, bool isGroup, idT id)
    {
        // Preguntamos si el inicio es incorrect y lanzamos un error si es asi
        if (range.peak() == range.end())
            throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);
        if (*(range.peak()) == charT('|'))
            throw myregex::basic_regex_error<charT>("bad start or operation", range.position(range.peak()), range, 14);
        std::vector<size_t> ends = {};
        // Obtenemos el estado final y lo agregamos
        size_t q0 = nfa.Q_nfa.size();
        nfa.Q_nfa.push_back({});
        // Obtenemos el primer estado
        // Agregamos una ∊-transition hacia ese estado
        nfa.Q_transitions[{q0, -1ULL}].push_back(nfa.Q_nfa.size());
        // Guardamos el estado final del resultado de la cadena
        if (range.peak() == range.end() || *range.peak() == charT(')'))
            throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);
        ends.push_back(basic_builder<charT, idT>::_S_build_nfa_parser_regular_expresions_basic(range, nfa, isGroup, id));
        // Repite la condicion en caso de ser una operacion or
        while (range.peak() < range.end() && *(range.peak()) == charT('|'))
        {
            range.next();
            if (range.peak() == range.end() || *range.peak() == charT(')'))
                throw myregex::basic_regex_error<charT>("end termination for regular expresion", range.position(range.end(), -1ULL), range, 0);
            // Agregamos una ∊-transition hacia ese estado
            nfa.Q_transitions[{q0, -1ULL}].push_back(nfa.Q_nfa.size());
            // Guardamos el estado final del resultado de la cadena
            ends.push_back(basic_builder<charT, idT>::_S_build_nfa_parser_regular_expresions_basic(range, nfa, isGroup, id));
            // Si se detuvo en una tuberia volver a realizar el procedimiento
        }
        // Para cada estado final se le agregara una ∊-transition al estado de union de la tuberia
        for (auto &&i : ends)
            nfa.Q_transitions[{i, -1ULL}].push_back(nfa.Q_nfa.size());
        // Por ultimo generamos correctamente el estado
        if (!isGroup)
        {
            nfa.Q_nfa.push_back(myregex::state(id));
            // nfa.F_nfa.emplace(nfa.Q_nfa.size() - 1, id);
        }
        return q0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////basic_dfa/////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////

    template <typename charT, typename idT>
    std::set<size_t> basic_builder<charT, idT>::_S_elipson_cloursers(const std::set<size_t> &states, const basic_nfa<charT, idT> &nfa)
    {
        std::stack<size_t> stack_status;
        std::set<size_t> clourser = states;
        for (auto &&state : states)
            stack_status.push(state);
        while (!stack_status.empty())
        {
            size_t index = stack_status.top();
            stack_status.pop();
            typename basic_nfa<charT, idT>::Transitions::const_iterator iterator = nfa.Q_transitions.begin();
            if ((iterator = nfa.Q_transitions.find({index, -1ULL})) == nfa.Q_transitions.end())
                continue;
            for (auto &&state : iterator->second)
            {
                stack_status.push(state);
                clourser.insert(state);
            }
        }
        return clourser;
    }

    template <typename charT, typename idT>
    std::set<size_t> basic_builder<charT, idT>::_S_move(const std::set<size_t> &states, charT a, const basic_nfa<charT, idT> &nfa)
    {
        std::set<size_t> result = {};
        for (auto &&state : states)
        {
            typename basic_nfa<charT, idT>::Transitions::const_iterator iterator = nfa.Q_transitions.begin();
            if ((iterator = nfa.Q_transitions.find({state, myregex::basic_builder<charT, idT>::_S_transition(a)})) != nfa.Q_transitions.end())
                result.insert(iterator->second[0]);
        }
        return result;
    }

    template <typename charT, typename idT>
    myregex::basic_nfa<charT, idT> myregex::basic_builder<charT, idT>::build_nfa(const std::initializer_list<std::pair<idT, std::__cxx11::basic_string<charT>>> &list)
    {
        myregex::basic_nfa<charT, idT> nfa;
        nfa.begin_Q_nfa.push_back(0);
        nfa.Q_nfa.push_back({});
        for (auto &&expresion : list)
        {
            basic_string_range<charT> range = expresion.second;
            nfa.Q_transitions[{0, -1ULL}].push_back(basic_builder<charT, idT>::_S_build_nfa_parser_or_expresions(range, nfa, false, expresion.first));
        }
        return nfa;
    }
    template <typename charT, typename idT>
    myregex::basic_nfa<charT, idT> myregex::basic_builder<charT, idT>::build_nfa(const std::vector<std::pair<idT, std::__cxx11::basic_string<charT>>> &list)
    {
        myregex::basic_nfa<charT, idT> nfa;
        nfa.begin_Q_nfa.push_back(0);
        nfa.Q_nfa.push_back({});
        for (auto &&expresion : list)
        {
            basic_string_range<charT> range = expresion.second;
            nfa.Q_transitions[{0, -1ULL}].push_back(basic_builder<charT, idT>::_S_build_nfa_parser_or_expresions(range, nfa, false, expresion.first));
        }
        return nfa;
    }

    template <typename charT, typename idT>
    myregex::basic_dfa<charT, idT> myregex::basic_builder<charT, idT>::build_dfa(const basic_nfa<charT, idT> &nfa)
    {
        basic_dfa<charT, idT> dfa = basic_dfa<charT, idT>();
        std::set<size_t> q0 = basic_builder<charT, idT>::_S_elipson_cloursers({0}, nfa);
        std::map<std::set<size_t>, size_t> status_map = {{q0, 0}};
        std::queue<std::set<size_t>> queue_status;
        queue_status.push(q0);
        dfa.Q_dfa.push_back({});

        while (!queue_status.empty())
        {
            std::set<size_t> index = queue_status.front();
            queue_status.pop();
            size_t actual = status_map[index];

            for (auto &&state : index)
            {
                if (nfa.Q_nfa[state].valid())
                {
                    dfa.Q_dfa[actual] = nfa.Q_nfa[state];
                    break;
                }
            }

            for (auto &&caraceter : nfa.Q_dictionary)
            {
                std::set<size_t> newindex = basic_builder<charT, idT>::_S_move_elipson_cloursers(index, caraceter, nfa);
                if (newindex.empty())
                    continue;
                if (status_map.count(newindex) < 1)
                {
                    status_map[newindex] = dfa.Q_dfa.size();
                    dfa.Q_dfa.push_back({});
                    queue_status.push(newindex);
                }
                dfa.Q_transitions[{actual, caraceter}] = status_map[newindex];
            }
        }
        return dfa;
    }
    template <typename charT, typename idT>
    myregex::basic_table<charT, idT> myregex::basic_builder<charT, idT>::build_table(const basic_dfa<charT, idT> &dfa)
    {
        std::basic_allocator<size_t> ttable = std::basic_allocator<size_t>(dfa.size() * basic_table<charT, idT>::dictionary);
        for (size_t state = 0; state < dfa.status().size(); state++)
        {
            for (size_t letter = 0; letter < basic_table<charT, idT>::dictionary; letter++)
            {
                auto transition = dfa.transitions().find({state, letter});
                if (transition != dfa.transitions().end())
                    ttable[(state * basic_table<charT, idT>::dictionary) + letter] = transition->second;
                else
                    ttable[(state * basic_table<charT, idT>::dictionary) + letter] = -1ULL;
            }
        }
        return {std::move(dfa.Q_dfa), std::move(ttable)};
    }

    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////Funciones que requieren _S_move/////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////

} // namespace myregex

#endif