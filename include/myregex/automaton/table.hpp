#ifndef _TABLEDFA
#define _TABLEDFA

#include <corecrt.h>
#include <type_traits>
#include <iostream>
#include <math.h>
#include <myregex/utilities/selector.hpp>
#include <myregex/automaton/base/state.hpp>

#define DEBUG false

namespace myregex
{
    template <typename charT, typename idT>
    class basic_builder;

    template <typename charT, typename idT>
    class basic_table
    {
        static_assert(std::is_same_v<charT, char> || std::is_same_v<charT, wchar_t>, "Error: no se permiten tipos de datos que no sean de caracteres(solo char o wchar_t)");

    private:
        using Qtable = std::vector<myregex::state<idT>>;
        using Transitions = size_t *;
        Qtable Q_table;
        Transitions Q_transitions;
        inline static Transitions build(size_t);
        inline static void copy(basic_table &, const basic_table &);

    public:
        basic_table() : Q_transitions(nullptr), Q_table({}) {}
        basic_table(const std::vector<myregex::state<idT>>&);
        basic_table(std::vector<myregex::state<idT>>&&);
        basic_table(const basic_table &);
        basic_table(basic_table &&);
        basic_table &operator=(const basic_table &);
        basic_table &operator=(basic_table &&);
        inline const Qtable& status() const { return Q_table; }
        inline const Transitions &transitions() const { return Q_transitions; }
        inline static constexpr size_t dictionary = std::pow(256ULL, sizeof(charT));
        inline size_t size() const { return Q_table.size() * myregex::basic_table<charT, idT>::dictionary * sizeof(size_t); }
#if DEBUG
        void view() const;
#endif
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
typename myregex::basic_table<charT, idT>::Transitions myregex::basic_table<charT, idT>::build(size_t nstates)
{
    myregex::basic_table<charT, idT>::Transitions transitions;
    transitions = new size_t [nstates * myregex::basic_table<charT, idT>::dictionary];
    // for (size_t state = 0; state < nstates; state++)
    // {
    //     transitions[state] = new size_t[myregex::basic_table<charT, idT>::dictionary];
    //     for (size_t letter = 0; letter < myregex::basic_table<charT, idT>::dictionary; letter++)
    //         transitions[state][letter] = -1ULL;
    // }
    return transitions;
}

template <typename charT, typename idT>
void myregex::basic_table<charT, idT>::copy(myregex::basic_table<charT, idT> &destine, const myregex::basic_table<charT, idT> &sources)
{
    // destine.Q_transitions = new size_t *[sources.Q_table.size()];
    destine.Q_transitions = new size_t [sources.Q_table.size() * myregex::basic_table<charT, idT>::dictionary];
    for (size_t state = 0; state < sources.Q_table.size(); state++)
    {
        // destine.Q_transitions[state] = new size_t[myregex::basic_table<charT, idT>::dictionary];
        for (size_t letter = 0; letter < myregex::basic_table<charT, idT>::dictionary; letter++)
            destine.Q_transitions[(state * myregex::basic_table<charT, idT>::dictionary) + letter] = sources.Q_transitions[(state * myregex::basic_table<charT, idT>::dictionary) + letter];
    }
    destine.Q_table = sources.Q_table;
}

template <typename charT, typename idT>
myregex::basic_table<charT, idT>::basic_table(const std::vector<myregex::state<idT>>& status) : Q_transitions(myregex::basic_table<charT, idT>::build(status.size())),
                                                                              Q_table(status) { }
template <typename charT, typename idT>
myregex::basic_table<charT, idT>::basic_table(std::vector<myregex::state<idT>>&& status) : Q_transitions(myregex::basic_table<charT, idT>::build(status.size())),
                                                                         Q_table(std::move(status)) {}

template <typename charT, typename idT>
myregex::basic_table<charT, idT>::basic_table(const myregex::basic_table<charT, idT> &other) : Q_transitions(nullptr),
                                                                                     Q_table(other.Q_table)
{
    if (!other.Q_transitions)
        throw std::runtime_error("error: empty basic_allocator in copy contructor: basic_allocator(const std::basic_allocator& other)");
    myregex::basic_table<charT, idT>::copy(*this, other);
}
template <typename charT, typename idT>
myregex::basic_table<charT, idT>::basic_table(myregex::basic_table<charT, idT> &&other) : Q_transitions(nullptr)
{
    if (other.Q_transitions)
    {
        Q_transitions = other.Q_transitions;
        other.Q_transitions = nullptr;
        Q_table = std::move(other.Q_table);
    }
}

template <typename charT, typename idT>
myregex::basic_table<charT, idT> &myregex::basic_table<charT, idT>::operator=(const basic_table &other)
{
    if (&other != this)
    {
        if (!other.Q_transitions)
            throw std::runtime_error("error: empty basic_allocator in copy contructor: basic_allocator(const std::basic_allocator& other)");
        myregex::basic_table<charT, idT>::copy(*this, other);
    }
    return *this;
}
template <typename charT, typename idT>
myregex::basic_table<charT, idT> &myregex::basic_table<charT, idT>::operator=(basic_table &&other)
{
    if (&other != this)
    {
        if (other.Q_transitions)
        {
            Q_transitions = other.Q_transitions;
            other.Q_transitions = nullptr;
            Q_table = std::move(other.Q_table);
        }
    }
    return *this;
}
#if DEBUG
template <typename charT, typename idT>
void myregex::basic_table<charT, idT>::view() const
{
    std::selector<charT>::stream() << "Q_table: " << this->Q_table << std::endl
               << "Q_transitions:" << std::endl
               << '\t';

    for (size_t letter = 0; letter < myregex::basic_table<charT, idT>::dictionary; letter++)
        std::selector<charT>::stream() << charT(letter) << charT(' ');

    std::selector<charT>::stream() << std::endl;

    for (size_t state = 0; state < this->Q_table; state++)
    {
        std::selector<charT>::stream() << state << ":      ";
        for (size_t letter = 0; letter < myregex::basic_table<charT, idT>::dictionary; letter++)
            std::selector<charT>::stream() << (long long)(this->Q_transitions[state][letter]) << charT(' ');
        std::selector<charT>::stream() << std::endl;
    }
}
#endif
template <typename charT, typename idT>
myregex::basic_table<charT, idT>::~basic_table()
{
    if (this->Q_transitions != nullptr)
    {
        // for (size_t state = 0; state < Q_table.size(); state++)
        //     if (Q_transitions[state] != nullptr)
        //         delete[] Q_transitions[state];
        // delete[] Q_transitions;
        delete[] Q_transitions;
        Q_transitions = nullptr;
    }
}

#endif