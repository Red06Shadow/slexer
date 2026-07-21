#ifndef _DFA
#define _DFA

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stdint.h>
#include <corecrt.h>
#include <type_traits>
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
        basic_dfa(const Qdfa &_states, const Transitions &_transitions) : Q_transitions(_transitions), Q_dfa(_states) {}
        inline const Transitions &transitions() const { return Q_transitions; }
        inline const Qdfa &status() const { return Q_dfa; }
        size_t size() const;
        friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &out, basic_dfa<charT, idT> other)
        {
            out << charT('{');
            for (size_t state = 0;  state < other.Q_dfa.size(); state++)
            {
                if (other.Q_dfa[state].valid())
                {
                    if constexpr (std::is_enum_v<idT>)
                        out << static_cast<size_t>(other.Q_dfa[state].get());
                    else
                        out << other.Q_dfa[state].get();
                }
                else
                    out << "{}";
                out << charT((state >= other.Q_dfa.size() - 1ULL) ? '}':',');
            }
            out << charT(',') << std::endl
                << charT('{');
            for (typename Transitions::const_iterator it = other.Q_transitions.begin(); it != other.Q_transitions.end(); it++)
            {
                if (it != other.Q_transitions.begin())
                    out << charT(',');
                if constexpr (std::is_same_v<charT, char>)
                    out << std::endl << "    {{" << it->first.first << ", '" << charT(uint8_t(it->first.second)) << "'}, " << it->second << " }";
                else 
                    out << std::endl << L"    {{" << it->first.first << L", L'" << charT(static_cast<uint8_t>(it->first.second)) << L"'}, " << it->second << L" }";
            }
            out << std::endl
                << charT('}') << std::endl;
            return out;
        }
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
