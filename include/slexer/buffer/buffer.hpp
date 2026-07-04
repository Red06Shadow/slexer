#ifndef SLEXERBUFFER
#define SLEXERBUFFER

#include <stdexcept>
#include <iostream>
#include <fstream>

namespace slexer
{
    template <typename CharT>
    class basic_buffer
    {
    private:
        CharT *_M_buffer;
        bool _M_is_eof;
        unsigned int _M_size_allocator;
        unsigned int _M_size_string;
        unsigned int _M_position;

    public:
        basic_buffer() : _M_buffer(nullptr), _M_is_eof(true), _M_size_allocator(0), _M_size_string(0), _M_position(0) {}
        basic_buffer(unsigned int _max) : _M_buffer(new CharT[_max + 1]), _M_is_eof(false), _M_size_allocator(_max), _M_size_string(_max), _M_position(0) {}
        basic_buffer(const basic_buffer<CharT> &other);
        basic_buffer(basic_buffer<CharT> &&other) noexcept;
        basic_buffer<CharT> &operator=(const basic_buffer<CharT> &other);
        basic_buffer<CharT> &operator=(basic_buffer<CharT> &&other) noexcept;

        inline bool eof() const { return _M_is_eof; }
        inline unsigned int position() const { return _M_position; }
        inline unsigned int size() const { return _M_size_string; }
        inline unsigned int max() const { return _M_size_allocator; }

        inline bool isbegin() const { return !_M_position; }
        inline bool isend() const { return _M_position >= _M_size_string; }

#if DEBUG
        inline const CharT* buffer() const { return _M_buffer; }
#endif

        std::iterator_traits<CharT> begin() const { return std::iterator_traits<CharT>(_M_buffer); }
        std::iterator_traits<CharT> end() const { return std::iterator_traits<CharT>(_M_buffer + _M_size_string); }

        inline CharT next() { return (isend() ? CharT(0) : _M_buffer[_M_position++]); }
        inline CharT peak() { return (isend() ? CharT(0) : _M_buffer[_M_position]); }

        

        friend std::basic_ifstream<CharT>& operator>>(std::basic_ifstream<CharT>& in, basic_buffer<CharT>& data) {
            in.read(data._M_buffer, data.max());
            data._M_is_eof = in.eof();
            data._M_size_string = in.gcount();
            data._M_buffer[data._M_size_string] = 0;
            data._M_position = 0;
            return in;
        }
        friend std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& out, const basic_buffer<CharT>& data) {
            out << data._M_buffer;                                
            return out;
        }

        ~basic_buffer();
    };
    template <typename CharT>
    basic_buffer<CharT>::basic_buffer(const basic_buffer<CharT> &other) : _M_buffer(nullptr), _M_is_eof(true), _M_size_allocator(0), _M_size_string(0), _M_position(0)
    {
        if (other._M_buffer == nullptr)
            throw std::runtime_error("no value other buffer");
        this->_M_buffer = new CharT[other._M_size_allocator + 1];
        this->_M_buffer[other._M_size_allocator] = 0;
        this->_M_size_allocator = other._M_size_allocator;
        this->_M_position = other._M_position;
        this->_M_size_string = other._M_size_string;
        this->_M_is_eof = other._M_is_eof;
        for (unsigned int i = 0; i < this->_M_size_string; i++)
            this->_M_buffer[i] = other._M_buffer[i];
    }
    template <typename CharT>
    basic_buffer<CharT>::basic_buffer(basic_buffer<CharT> &&other) noexcept : _M_buffer(other._M_buffer), _M_is_eof(other._M_is_eof), _M_size_allocator(other._M_size_allocator), _M_size_string(other._M_size_string), _M_position(other._M_position) { other._M_buffer = nullptr; }
    template <typename CharT>
    basic_buffer<CharT> &basic_buffer<CharT>::operator=(const basic_buffer<CharT> &other)
    {
        if (this != &other)
        {
            if (other._M_buffer == nullptr)
                throw std::runtime_error("no value other buffer");
            this->_M_buffer = new CharT[other._M_size_allocator + 1];
            this->_M_buffer[other._M_size_allocator] = 0;
            this->_M_size_allocator = other._M_size_allocator;
            this->_M_position = other._M_position;
            this->_M_size_string = other._M_size_string;
            this->_M_is_eof = other._M_is_eof;
            for (unsigned int i = 0; i < this->_M_size_string; i++)
                this->_M_buffer[i] = other._M_buffer[i];
        }
        return *this;
    }
    template <typename CharT>
    basic_buffer<CharT> &basic_buffer<CharT>::operator=(basic_buffer<CharT> &&other) noexcept
    {
        if (this != &other)
        {
            this->_M_buffer = other._M_buffer;
            this->_M_size_allocator = other._M_size_allocator;
            this->_M_position = other._M_position;
            this->_M_size_string = other._M_size_string;
            this->_M_is_eof = other._M_is_eof;
            other._M_buffer = nullptr;
        }
        return *this;
    }
    template <typename CharT>
    basic_buffer<CharT>::~basic_buffer()
    {
        if (_M_buffer == nullptr)
            delete[] _M_buffer;
    }
} // namespace slexer

#endif