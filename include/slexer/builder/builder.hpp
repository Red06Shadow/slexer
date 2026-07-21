#ifndef SLEXERBUILDER
#define SLEXERBUILDER

#include <myregex/builder/builder.hpp>
#include <slexer/main/main.hpp>

namespace slexer
{
    template <typename charT, typename idT>
    struct basic_rule
    {
        idT _id;
        const std::basic_string<charT> &_regex;
        typename slexer::basic_lexer<charT, idT>::handle _funtion;
    };

    template <typename charT, typename idT>
    class basic_builder
    {
    private:
        inline static void _S_option(std::vector<
                                                 myregex::basic_table<charT,
                                                 typename slexer::basic_lexer<charT, idT>::_I_idT>> &,
                                     const std::initializer_list<basic_rule<charT, idT>> &);

    private:
        /// @brief Tabla de transiciones para todos los casos
        std::vector<
            myregex::basic_table<charT,
                               typename slexer::basic_lexer<charT, idT>::_I_idT>>
            _M_group_tables;
        unsigned int _M_buffer_size;

    public:
        basic_builder(const std::initializer_list<basic_rule<charT, idT>> &rules) : _M_group_tables()
        {
            _M_group_tables.reserve(8);
            basic_builder<charT, idT>::_S_option(_M_group_tables, rules);
            _M_buffer_size = 4096;
        }
        basic_builder<charT, idT> &option(const std::initializer_list<basic_rule<charT, idT>> &rules)
        {
            basic_builder<charT, idT>::_S_option(_M_group_tables, rules);
            return *this;
        }
        basic_builder<charT, idT> &buffer(unsigned int size)
        {
            _M_buffer_size = size;
            return *this;
        }
        basic_lexer<charT, idT> build() { return basic_lexer<charT, idT>(_M_group_tables, _M_buffer_size); }
        ~basic_builder() {}
    };
    template <typename charT, typename idT>
    void basic_builder<charT, idT>::_S_option(std::vector<
                                                 myregex::basic_table<charT,
                                                 typename slexer::basic_lexer<charT, idT>::_I_idT>> &_M_regex_reference, const std::initializer_list<basic_rule<charT, idT>> &_M_rules)
    {
        std::vector<std::pair<typename slexer::basic_lexer<charT, idT>::_I_idT, std::string>> regexs;
        regexs.reserve(_M_rules.size());
        for (auto &&[_id, _regex, _funtion] : _M_rules)
            regexs.push_back({typename slexer::basic_lexer<charT, idT>::_I_idT(_id, _funtion), _regex});

        _M_regex_reference.push_back(
            myregex::basic_builder<charT, typename slexer::basic_lexer<charT, idT>::_I_idT>::build_table(
                myregex::basic_builder<charT, typename slexer::basic_lexer<charT, idT>::_I_idT>::build_dfa(
                    myregex::basic_builder<charT, typename slexer::basic_lexer<charT, idT>::_I_idT>::build_nfa(regexs))));
    }

    template <typename idT>
    using builder = basic_builder<char, idT>;
    template <typename idT>
    using wbuilder = basic_builder<wchar_t, idT>;
} // namespace slexer

#endif