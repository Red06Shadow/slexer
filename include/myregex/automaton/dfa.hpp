#ifndef _DFA
#define _DFA

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <corecrt.h>
#include <type_traits>
#include <myregex/utilities/selector.hpp>
#include <myregex/automaton/base/state.hpp>

#define DEBUG false

namespace myregex
{
    template <typename charT, typename idT>
    class basic_builder;

    template <typename charT, typename idT>
    class basic_dfa
    {
    public: //  Structs, Enums, Class
        static_assert(std::is_same_v<charT, char> || std::is_same_v<charT, wchar_t>, "Error: no se permiten tipos de datos que no sean de caracteres(solo char o wchar_t)");
        using Qdfa = std::vector<myregex::state<idT>>;
        using Transitions = std::map<std::pair<size_t, charT>, size_t>;

    private:
        /// @brief Estados
        Qdfa Q_dfa;
        /// @brief Transiciones
        Transitions Q_transitions;

    public:
        basic_dfa() : Q_transitions({}), Q_dfa({}) {}
        inline const Transitions &transitions() const { return Q_transitions; }
        inline const Qdfa &status() const { return Q_dfa; }
        size_t size() const;
#if DEBUG
        void view() const;
#endif
        ~basic_dfa() {}
        friend basic_builder<charT, idT>;
    };
    template <typename charT, typename idT>
    size_t basic_dfa<charT, idT>::size() const
    {
        size_t _size = 0;
        _size += sizeof(size_t);
        _size += Q_dfa.size() * (sizeof(myregex::state<idT>) * 2) + sizeof(Qdfa);
        _size += Q_transitions.size() * (sizeof(size_t) * 2 + sizeof(charT)) + sizeof(Transitions);
        return _size;
    }
#if DEBUG
    template <typename charT, typename idT>
    void basic_dfa<charT, idT>::view() const
    {
        std::selector<charT>::stream() << "Transiciones (Q_transitions): " << std::endl
                                  << '{' << std::endl;

        for (auto &&[key, state] : Q_transitions)
            std::selector<charT>::stream() << "    {" << key.first << ", " << charT(key.second) << "} -> { " << state << " }" << std::endl;
        std::selector<charT>::stream() << '}' << std::endl;
    }
#endif
    template <typename idT>
    using CompatibleDfa = basic_dfa<char, idT>;
    template <typename idT>
    using dfa = basic_dfa<char, idT>;
    /////////////////////////////////////////////////////////////////////
    template <typename idT>
    using UnicodeDfa = basic_dfa<wchar_t, idT>;
    template <typename idT>
    using wdfa = basic_dfa<wchar_t, idT>;
} // namespace myregex

#endif
