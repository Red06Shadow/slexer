#ifndef SLEXERBUFFER
#define SLEXERBUFFER
//<--...
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stdint.h>

namespace slexer
{

    template <typename charT>
    class basic_buffer
    {
    private:
        charT *_M_allocator;
        uint32_t _M_capacity;
        bool _M_is_eof;
        uint32_t _M_queue;
        uint32_t _M_head;
        uint32_t _M_start;
        uint32_t _M_count;

        void _M_realloc();

    public:
        basic_buffer() : _M_allocator(nullptr), _M_capacity(0), _M_is_eof(false), _M_queue(0), _M_head(0), _M_count(0), _M_start(0) {}
        basic_buffer(uint32_t size) : _M_allocator(new charT[size]), _M_capacity(size), _M_is_eof(false), _M_queue(0), _M_head(0), _M_count(0), _M_start(size) {}

        basic_buffer(const basic_buffer<charT> &other);
        basic_buffer(basic_buffer<charT> &&other) noexcept;
        basic_buffer<charT> &operator=(const basic_buffer<charT> &other);
        basic_buffer<charT> &operator=(basic_buffer<charT> &&other) noexcept;

        void push(charT c);
        charT peak();
        charT pop();
        void back(uint32_t n);
        friend std::basic_ifstream<charT> &operator>>(std::basic_ifstream<charT> &in, basic_buffer &buff)
        {
            if (buff._M_count == 0 && buff._M_queue == buff._M_start)
                buff._M_realloc();

            if (buff._M_start == buff._M_capacity)
            {
                in.read(buff._M_allocator, buff._M_capacity);
                buff._M_start = 0;
                buff._M_count = in.gcount();
            }
            else
            {
                if (buff._M_queue > buff._M_start)
                {
                    in.read(buff._M_allocator + buff._M_queue, buff._M_capacity - buff._M_queue);
                    buff._M_count += in.gcount();
                    if (!(buff._M_is_eof = in.eof()) && buff._M_start > 0)
                    {
                        in.read(buff._M_allocator, buff._M_start);
                        buff._M_count += in.gcount();
                    }
                }
                else
                {
                    in.read(buff._M_allocator + buff._M_queue, buff._M_start - buff._M_queue);
                    buff._M_count += in.gcount();
                }
            }
            buff._M_queue = buff._M_start;
            buff._M_is_eof = in.eof();
            in.clear();
            return in;
        }
        friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &out, const basic_buffer<charT> &_buffer)
        {
            out.write(_buffer._M_allocator, _buffer.capacity());
            return out;
        }
        inline void update() { _M_start = _M_head; }
        inline void copy(std::basic_string<charT> &, uint32_t, uint32_t);
        inline bool eof() const { return _M_is_eof; }
        inline bool end() const { return _M_is_eof && _M_count == 0; }
        inline bool empty() const { return _M_count == 0; }
        inline uint32_t size() const { return _M_count; }
        inline uint32_t capacity() const { return _M_capacity; }
        inline bool full() const { return _M_count == _M_capacity; }
        inline std::string_view to_string() const { return std::string_view(_M_allocator, _M_capacity); }
        ~basic_buffer();
    };

    template <typename charT>
    basic_buffer<charT>::basic_buffer(const basic_buffer<charT> &other) : _M_allocator(nullptr),
                                                                          _M_capacity(other._M_capacity),
                                                                          _M_is_eof(other._M_is_eof),
                                                                          _M_queue(other._M_queue),
                                                                          _M_head(other._M_head),
                                                                          _M_count(other._M_count),
                                                                          _M_start(other._M_start)
    {
        this->_M_allocator = new charT[other._M_capacity];
        for (size_t i = 0; i < other._M_capacity; i++)
            this->_M_allocator[i] = other._M_allocator[i];
    }

    template <typename charT>
    basic_buffer<charT>::basic_buffer(basic_buffer<charT> &&other) noexcept : _M_allocator(std::move(other._M_allocator)),
                                                                              _M_capacity(other._M_capacity),
                                                                              _M_is_eof(other._M_is_eof),
                                                                              _M_queue(other._M_queue),
                                                                              _M_head(other._M_head),
                                                                              _M_count(other._M_count),
                                                                              _M_start(other._M_start)
    {
        other._M_allocator = nullptr;
    }

    template <typename charT>
    basic_buffer<charT> &basic_buffer<charT>::operator=(const basic_buffer<charT> &other)
    {
        if (this != &other)
        {
            _M_capacity = other._M_capacity;
            _M_is_eof = other._M_is_eof;
            _M_queue = other._M_queue;
            _M_head = other._M_head;
            _M_count = other._M_count;
            _M_start = other._M_start;
            this->_M_allocator = new charT[other._M_capacity];
            for (size_t i = 0; i < other._M_capacity; i++)
                this->_M_allocator[i] = other._M_allocator[i];
        }
        return *this;
    }
    template <typename charT>
    basic_buffer<charT> &basic_buffer<charT>::operator=(basic_buffer<charT> &&other) noexcept
    {
        if (this != &other)
        {
            _M_capacity = other._M_capacity;
            _M_is_eof = other._M_is_eof;
            _M_queue = other._M_queue;
            _M_head = other._M_head;
            _M_count = other._M_count;
            _M_start = other._M_start;
            this->_M_allocator = other._M_allocator;
            other._M_allocator = nullptr;
        }
        return *this;
    }

    template <typename charT>
    void basic_buffer<charT>::push(charT c)
    {
        if (_M_count == _M_capacity)
            _M_realloc();
        _M_allocator[_M_queue] = c;
        _M_queue = (_M_queue + 1) % _M_capacity;
        _M_count++;
    }

    template <typename charT>
    void basic_buffer<charT>::_M_realloc()
    {
        uint32_t _new_capacity = _M_capacity * 2;
        charT *_new_allocator = new charT[_new_capacity];
        for (uint32_t i = 0; i < _M_capacity; i++)
        {
            _new_allocator[i] = _M_allocator[_M_queue];
            _M_queue = (_M_queue + 1) % _M_capacity;
        }
        _M_head = _M_queue = _M_capacity;
        _M_start = 0;
        _M_capacity = _new_capacity;
        delete _M_allocator;
        _M_allocator = _new_allocator;
    }

    template <typename charT>
    inline void basic_buffer<charT>::copy(std::basic_string<charT> &str, uint32_t start, uint32_t end)
    {
        while (start != end)
        {
            str.push_back(_M_allocator[start]);
            start = (start + 1) % _M_capacity;
        }
    }

    template <typename charT>
    charT basic_buffer<charT>::pop()
    {
        if (_M_count == 0)
            return -1;
        charT c = _M_allocator[_M_head];
        _M_head = (_M_head + 1) % _M_capacity;
        _M_count--;
        return c;
    }

    template <typename charT>
    void basic_buffer<charT>::back(uint32_t n) {
        if (_M_count + n > _M_capacity)
            n = _M_capacity - _M_count;
        _M_count += n;
        //////////////////////////////////////////
        if ((_M_head - n) > _M_capacity)
            _M_head = _M_capacity - (n - _M_head);
        else
            _M_head -= n;
    }

    template <typename charT>
    charT basic_buffer<charT>::peak()
    {
        if (_M_count == 0)
            return -1;
        return _M_allocator[_M_head];
    }

    template <typename charT>
    basic_buffer<charT>::~basic_buffer()
    {
        if (_M_allocator != nullptr)
        {
            delete[] _M_allocator;
            _M_allocator = nullptr;
        }
    }

    // template <typename charT>
    // class basic_buffer
    // {
    // private:
    //     charT *_M_buffer;
    //     bool _M_is_eof;
    //     uint32_t _M_capacity;
    //     ///////////////////////////
    //     // uint32_t _M_head;
    //     // uint32_t _M_reader_position;
    //     // uint32_t _M_reader_start;
    //     uint32_t _M_point_start_write;

    //     inline static bool is_overflow(uint32_t adder1, uint32_t adder2) { return (adder1 + adder2) < adder1; }
    //     inline static bool is_underflow(uint32_t minuend, uint32_t subtrahend) { return (minuend - subtrahend) > minuend; }
    //     inline void overwrite(uint32_t epw, std::basic_ifstream<charT>& in) {
    //         if (is_underflow(epw, _M_point_start_write))
    //             in.read(_buffer._M_buffer + _M_point_start_write, _M_capacity - _M_point_start_write).read(_buffer._M_buffer, epw);
    //         else
    //             in.read(_buffer._M_buffer + _M_point_start_write, epw - _M_point_start_write);
    //         _M_point_start_write = epw;
    //     }
    //     inline static uint32_t overflow(uint32_t point, uint32_t size) { return point % size; }

    // public:
    //     basic_buffer() :
    //     _M_buffer(nullptr), _M_is_eof(true), _M_capacity(0) {}
    //     basic_buffer(uint32_t _max) : _M_buffer(new charT[_max]),
    //     _M_is_eof(false), _M_capacity(_max) {}

    //     basic_buffer(const basic_buffer<charT> &other);
    //     basic_buffer(basic_buffer<charT> &&other) noexcept;
    //     basic_buffer<charT> &operator=(const basic_buffer<charT> &other);
    //     basic_buffer<charT> &operator=(basic_buffer<charT> &&other) noexcept;

    //     inline bool eof() const { return _M_is_eof; }

    //     friend std::basic_ifstream<charT>& operator>>(std::basic_ifstream<charT>& in, basic_buffer<charT>& _buffer) {
    //         // if (_buffer._M_head)
    //         // {

    //         // }
    //         // else
    //         // {
    //         //     in.read(_buffer._M_buffer, _buffer.max());
    //         //     _buffer._M_is_eof = in.eof();
    //         //     _buffer._M_head = in.gcount();
    //         //     _buffer._M_buffer[_buffer._M_head] = 0;
    //         //     _buffer._M_reader_position = 0;
    //         //     in.clear();
    //         // }
    //         return in;
    //     }
    //     friend std::basic_ostream<charT>& operator<<(std::basic_ostream<charT>& out, const basic_buffer<charT>& _buffer) {
    //         out << _buffer._M_buffer;
    //         return out;
    //     }

    //     ~basic_buffer();
    // };
    // template <typename charT>
    // basic_buffer<charT>::basic_buffer(const basic_buffer<charT> &other) : _M_buffer(nullptr), _M_is_eof(true), _M_capacity(0), _M_head(0), _M_reader_position(0)
    // {
    //     if (other._M_buffer == nullptr)
    //         throw std::runtime_error("no value other buffer");
    //     this->_M_buffer = new charT[other._M_capacity];
    //     this->_M_capacity = other._M_capacity;
    //     this->_M_is_eof = other._M_is_eof;
    //     for (uint32_t i = 0; i < this->_M_capacity; i++)
    //         this->_M_buffer[i] = other._M_buffer[i];
    // }
    // template <typename charT>
    // basic_buffer<charT>::basic_buffer(basic_buffer<charT> &&other) noexcept : _M_buffer(std::move(other._M_buffer)), _M_is_eof(other._M_is_eof), _M_capacity(other._M_capacity) { other._M_buffer = nullptr; }
    // template <typename charT>
    // basic_buffer<charT> &basic_buffer<charT>::operator=(const basic_buffer<charT> &other)
    // {
    //     if (this != &other)
    //     {
    //         if (other._M_buffer == nullptr)
    //             throw std::runtime_error("no value other buffer");
    //         this->_M_buffer = new charT[other._M_capacity];
    //         this->_M_capacity = other._M_capacity;
    //         this->_M_is_eof = other._M_is_eof;
    //         for (uint32_t i = 0; i < this->_M_capacity; i++)
    //             this->_M_buffer[i] = other._M_buffer[i];
    //     }
    //     return *this;
    // }
    // template <typename charT>
    // basic_buffer<charT> &basic_buffer<charT>::operator=(basic_buffer<charT> &&other) noexcept
    // {
    //     if (this != &other)
    //     {
    //         this->_M_buffer = std::move(other._M_buffer);
    //         this->_M_capacity = other._M_capacity;
    //         this->_M_is_eof = other._M_is_eof;
    //         other._M_buffer = nullptr;
    //     }
    //     return *this;
    // }
    // template <typename charT>
    // basic_buffer<charT>::~basic_buffer()
    // {
    //     if (_M_buffer == nullptr)
    //         delete[] _M_buffer;
    // }
} // namespace slexer
//<--...
#endif