#ifndef SLEXTOKENSTREAM
#define SLEXTOKENSTREAM

#include <vector>
#include <slexer/token/token.hpp>

namespace slexer
{
    template <typename charT, typename idT>
    class basic_tokenstream
    {
    private:
        static_assert(std::is_unsigned_v<idT> | std::is_enum_v<idT>, "in basic_tokenstream: do you no have template with value is not a unsigned integer");
        std::vector<slexer::basic_token<charT, idT>> _M_tokens;
    
    public:
        using const_iterator = typename std::vector<slexer::basic_token<charT, idT>>::const_iterator; 
    public:
        basic_tokenstream() : _M_tokens() {}
        inline void push(const slexer::basic_token<charT, idT>& token) { _M_tokens.push_back(token); }
        inline const slexer::basic_token<charT, idT>& back() const { return _M_tokens.back(); }
        inline basic_tokenstream::const_iterator begin() const { return _M_tokens.begin(); }
        inline basic_tokenstream::const_iterator end() const { return _M_tokens.end(); }
        inline const slexer::basic_token<charT, idT>& operator[](size_t pos) const { return _M_tokens[pos]; }
        inline size_t size() const { return _M_tokens.size(); }
        inline bool empty() const { return _M_tokens.empty(); }
        ~basic_tokenstream() {}
    };

    template <typename idT>
    using tokenstream = basic_tokenstream<char, idT>;
    template <typename idT>
    using wtokenstream = basic_tokenstream<wchar_t, idT>;
    
} // namespace slexer


#endif