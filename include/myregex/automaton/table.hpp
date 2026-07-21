#ifndef _TABLEDFA
#define _TABLEDFA

#include <myregex/automaton/base/state.hpp>
#include <utilities/memory.hpp>
//<--...
#include <corecrt.h>
#include <type_traits>
#include <iostream>
#include <vector>
#include <math.h>

#define DEBUG false

namespace myregex
{
    template <typename charT, typename idT>
    class basic_builder;

    template <typename charT, typename idT>
    class basic_table
    {
        static_assert(std::is_same_v<charT, char> || std::is_same_v<charT, wchar_t>, "Error: no se permiten tipos de datos que no sean de caracteres(solo char o wchar_t)");

    public:
        using Qtable = std::vector<myregex::state<idT>>;
        using Transitions = std::basic_allocator<size_t>;

    private:
        Qtable Q_table;
        Transitions Q_transitions;

    public:
        basic_table() : Q_transitions(), Q_table() {}
        basic_table(const Qtable &_states, const Transitions &_transitions) : Q_transitions(_transitions), Q_table(_states) {}
        basic_table(Qtable &&_states, Transitions &&_transitions) : Q_transitions(std::move(_transitions)), Q_table(std::move(_states)) {}
        basic_table(const std::vector<myregex::state<idT>> &status) : Q_transitions(status.size() * myregex::basic_table<charT, idT>::dictionary), Q_table(status) {}
        basic_table(std::vector<myregex::state<idT>> &&status) : Q_transitions(status.size() * myregex::basic_table<charT, idT>::dictionary), Q_table(std::move(status)) {}
        basic_table(const basic_table &other) : Q_transitions(other.Q_transitions), Q_table(other.Q_table) {}
        basic_table(basic_table &&other) : Q_transitions(std::move(other.Q_transitions)), Q_table(std::move(other.Q_table)) {}
        basic_table &operator=(const basic_table &);
        basic_table &operator=(basic_table &&);
        inline const Qtable &status() const { return Q_table; }
        inline const Transitions &transitions() const { return Q_transitions; }
        inline static constexpr size_t dictionary = std::pow(256ULL, sizeof(charT));
        inline size_t size() const { return Q_table.size() * myregex::basic_table<charT, idT>::dictionary * sizeof(size_t) + sizeof(Transitions); }
        friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &out, basic_table<charT, idT> other)
        {
            out << charT('{');
            for (size_t state = 0; state < other.Q_table.size(); state++)
            {
                if (other.Q_table[state].valid())
                {
                    if constexpr (std::is_enum_v<idT>)
                        out << static_cast<size_t>(other.Q_table[state].get());
                    else
                        out << other.Q_table[state].get();
                }
                else
                    out << charT('{') << charT('}');
                out << charT((state >= other.Q_table.size() - 1ULL) ? '}' : ',');
            }
            out << charT(',') << std::endl
                << charT('{');
            for (size_t state = 0; state < other.Q_table.size(); state++)
            {
                for (size_t letter = 0; letter < myregex::basic_table<charT, idT>::dictionary; letter++)
                {
                    if constexpr (std::is_same_v<charT, char>)
                        out << (long long)(other.Q_transitions[(state * myregex::basic_table<charT, idT>::dictionary) + letter]) << "ULL" << charT(',');
                    else
                        out << (long long)(other.Q_transitions[(state * myregex::basic_table<charT, idT>::dictionary) + letter]) << L"ULL" << charT(',');
                }
                out << std::endl;
            }
            out << charT('}');
            return out;
        }
        ~basic_table();
        friend basic_builder<charT, idT>;
    };
    template <typename idT>
    using CompatibleTable = basic_table<char, idT>;
    template <typename idT>
    using table = basic_table<char, idT>;
    /////////////////////////////////////////////////////////////////////
    template <typename idT>
    using UnicodeTable = basic_table<wchar_t, idT>;
    template <typename idT>
    using wtable = basic_table<wchar_t, idT>;
} // namespace myregex

template <typename charT, typename idT>
myregex::basic_table<charT, idT> &myregex::basic_table<charT, idT>::operator=(const basic_table &other)
{
    if (&other != this)
    {
        Q_transitions = other.Q_transitions;
        Q_table = other.Q_table;
    }
    return *this;
}
template <typename charT, typename idT>
myregex::basic_table<charT, idT> &myregex::basic_table<charT, idT>::operator=(basic_table &&other)
{
    if (&other != this)
    {
        Q_transitions = std::move(other.Q_transitions);
        Q_table = std::move(other.Q_table);
    }
    return *this;
}
template <typename charT, typename idT>
myregex::basic_table<charT, idT>::~basic_table() {}
//<--...
#endif