#ifndef _NFA
#define _NFA

#include <iostream>
#include <vector>
#include <type_traits>
#include <stdexcept>
#include <map>
#include <set>
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
        basic_nfa(const Qnfa &_states, const SQnfa &_starts, const Transitions &_transitions, const Dictionary &_dictionary) : Q_nfa(_states), begin_Q_nfa(_starts), Q_transitions(_transitions), Q_dictionary(_dictionary) {}
        inline const Qnfa &status() const { return Q_nfa; }
        inline const SQnfa &begin_status() const { return begin_Q_nfa; }
        inline const Transitions &transitions() const { return Q_transitions; }
        inline const Dictionary &dictionary() const { return Q_dictionary; }
        friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &out, basic_nfa<charT, idT> other)
        {
            out << charT('{');
            for (size_t state = 0; state < other.Q_nfa.size(); state++)
            {
                if (other.Q_nfa[state].valid())
                {
                    if constexpr (std::is_enum_v<idT>)
                        out << static_cast<size_t>(other.Q_nfa[state].get());
                    else
                        out << other.Q_nfa[state].get();
                }
                else
                    out << charT('{') << charT('}');
                out << charT((state >= other.Q_nfa.size() - 1ULL) ? '}' : ',');
            }
            out << charT(',') << std::endl
                << charT('{');
            for (size_t state = 0; state < other.begin_Q_nfa.size(); state++)
                out << other.begin_Q_nfa[state] << charT((state >= (other.begin_Q_nfa.size() - 1ULL)) ? '}' : ',');
            out << charT(',') << std::endl
                << charT('{');
            for (typename Transitions::const_iterator it = other.Q_transitions.begin(); it != other.Q_transitions.end(); it++)
            {
                if (it != other.Q_transitions.begin())
                    out << charT(',');
                if constexpr (std::is_same_v<charT, char>)
                    out << std::endl << "    {{" << it->first.first << ',' << (long long)it->first.second << "ULL},{";
                else
                    out << std::endl << L"    {{" << it->first.first << L',' << (long long)it->first.second << L"ULL},{";
                
                for (size_t next = 0; next < it->second.size(); next++)
                    out << it->second[next] << charT(next >= it->second.size() - 1ULL ? '}' : ',');
                out << charT('}');
            }
            out << std::endl
                << charT('}') << charT(',') << std::endl
                << charT('{');
            for (typename Dictionary::const_iterator it = other.Q_dictionary.begin(); it != other.Q_dictionary.end(); it++)
            {
                if (it != other.Q_dictionary.begin())
                    out << charT(',');
                if constexpr (std::is_same_v<charT, char>)
                    out << '\'' << *it << '\'';
                else
                    out  << L'L' << L'\'' << *it << L'\'';
            }
            out << charT('}');
            return out;
        }
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