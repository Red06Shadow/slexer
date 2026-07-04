#ifndef _NFA
#define _NFA

#include <iostream>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <map>
#include <set>
#include <myregex/utilities/selector.hpp>
#include <myregex/automaton/base/state.hpp>

#define DEBUG false

namespace myregex
{

    template <typename charT, typename idT>
    class basic_builder;

    template <typename charT, typename idT>
    class basic_nfa
    {
    public:
        static_assert(std::is_same_v<charT, char> || std::is_same_v<charT, wchar_t>, "Error: no se permiten tipos de datos que no sean de caracteres(solo char o wchar_t)");
        // Estados
        using Qnfa = std::vector<myregex::state<idT>>;
        using SQnfa = std::vector<size_t>;
        // Transicion estandar
        using Transitions = std::map<std::pair<size_t, size_t>, std::vector<size_t>>;
        using Dictionary = std::set<charT>;

    private:
        /// @brief Estados
        Qnfa Q_nfa;
        /// @brief Estados de inicio
        SQnfa begin_Q_nfa;
        /// @brief Transiciones
        Transitions Q_transitions;
        /// @brief Diccionario
        Dictionary Q_dictionary;
        friend basic_builder<charT, idT>;

    public:
        basic_nfa() : Q_nfa(), begin_Q_nfa(), Q_transitions(), Q_dictionary() {}
        inline const Qnfa &status() const { return Q_nfa; }
        inline const SQnfa &begin_status() const { return begin_Q_nfa; }
        inline const Transitions &transitions() const { return Q_transitions; }
        inline const Dictionary &dictionary() const { return Q_dictionary; }
    #if DEBUG
        void view() const;
    #endif
        size_t size() const;
        ~basic_nfa() {}
    };

    template <typename charT, typename idT>
    size_t basic_nfa<charT, idT>::size() const
    {
        size_t _size = 0;
        _size += Q_nfa.size() * sizeof(myregex::state<idT>) + sizeof(Qnfa);
        _size += Q_dictionary.size() * sizeof(charT) + sizeof(Dictionary);
        _size += begin_Q_nfa.size() * sizeof(size_t) + sizeof(SQnfa);
        _size += Q_transitions.size() * (sizeof(size_t) * 2 + sizeof(charT)) + sizeof(Transitions);
        return _size;
    }
#if DEBUG
    template <typename charT, typename idT>
    void basic_nfa<charT, idT>::view() const
    {
        std::selector<charT>::stream() << "Diccionario (dictionary): { ";

        for (auto &&letter : Q_dictionary)
            std::selector<charT>::stream() << letter << ' ';

        std::selector<charT>::stream() << '}' << std::endl
                   << "Transiciones (Q_transitions): " << std::endl
                   << '{' << std::endl;
        for (auto &&[key, states] : Q_transitions)
        {
            if (key.second == -1ULL)
            {
                std::selector<charT>::stream() << "    {" << key.first << "} -> { ";
                for (size_t state = 0; state < states.size(); state++)
                    std::selector<charT>::stream() << states[state] << (state >= states.size() - 1ULL ? " }" : ", ");
                std::selector<charT>::stream() << std::endl;
            }
            else
                std::selector<charT>::stream() << "    {" << key.first << ", " << charT(key.second) << "} -> { " << states.back() << " }" << std::endl;
        }
        std::selector<charT>::stream() << '}' << std::endl 
                   << "Estados de Inicio (begin_Q_nfa): { ";

        for (auto &&state : begin_Q_nfa)
            std::selector<charT>::stream() << state << ' ';
    }
#endif
    template <typename idT>
    using CompatibleNfa = myregex::basic_nfa<char, idT>;
    template <typename idT>
    using nfa = myregex::basic_nfa<char, idT>;
    /////////////////////////////////////////////////////////////////////
    template <typename idT>
    using UnicodeNfa = myregex::basic_nfa<wchar_t, idT>;
    template <typename idT>
    using wnfa = myregex::basic_nfa<wchar_t, idT>;
} // namespace myregex
#endif