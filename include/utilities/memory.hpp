#ifndef _GLIBCXX_MEMORY_ME
#define _GLIBCXX_MEMORY_ME
//<--...
#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>

namespace std
{
    template <typename type>
    class basic_ptr
    {
    private:
        type *_M_ptr;

    public:
        basic_ptr() : _M_ptr(nullptr) {}
        basic_ptr(const type &value) : _M_ptr(new type(value)) {}
        basic_ptr(const std::basic_ptr<type> &other) : _M_ptr(nullptr)
        {
            if (other._M_ptr)
                this->_M_ptr = new type(*other._M_ptr);
        }
        basic_ptr(std::basic_ptr<type> &&other) noexcept : _M_ptr(other._M_ptr) { other._M_ptr = nullptr; }
        std::basic_ptr<type> &operator=(std::nullptr_t)
        {
            this->reset();
            return *this;
        }
        std::basic_ptr<type> &operator=(const type &other)
        {
            if (this->_M_ptr)
                delete this->_M_ptr;
            this->_M_ptr = new type(other);
            return *this;
        }
        std::basic_ptr<type> &operator=(const std::basic_ptr<type> &other)
        {
            if (&other != this)
            {
                if (this->_M_ptr)
                    delete this->_M_ptr;
                if (other._M_ptr)
                    this->_M_ptr = new type(*other._M_ptr);
                else
                    this->_M_ptr = nullptr;
            }
            return *this;
        }
        std::basic_ptr<type> &operator=(std::basic_ptr<type> &&other) noexcept
        {
            if (&other != this)
            {
                if (this->_M_ptr)
                    delete this->_M_ptr;
                this->_M_ptr = other._M_ptr;
                other._M_ptr = nullptr;
            }
            return *this;
        }
        _GLIBCXX23_CONSTEXPR type operator*(int)
            const noexcept(noexcept(*std::declval<type>()))
        {
            return *get();
        }

        _GLIBCXX23_CONSTEXPR type *operator->()
            const noexcept { return get(); }

        _GLIBCXX23_CONSTEXPR type *get() const noexcept
        {
            return _M_ptr;
        }

        void reset()
        {
            if (_M_ptr)
                delete _M_ptr;
            _M_ptr = nullptr;
        }
        ~basic_ptr()
        {
            if (_M_ptr)
                delete _M_ptr;
            _M_ptr = nullptr;
        }
    };

    template <typename type>
    class basic_allocator
    {
    public:
    private:
        type *_M_ptr;
        size_t _M_size;

    public:
        basic_allocator() : _M_ptr(nullptr), _M_size(0) {}
        basic_allocator(const std::initializer_list<type> &list);
        basic_allocator(const std::vector<type> &list);
        explicit basic_allocator(size_t size) : _M_size(size), _M_ptr(new type[size]) {}
        basic_allocator(const std::basic_allocator<type> &other) : _M_ptr(nullptr), _M_size(0)
        {
            if (other._M_ptr && other._M_size > 0)
            {
                _M_ptr = new type[other._M_size];
                std::copy(other._M_ptr, other._M_ptr + other._M_size, _M_ptr);
                _M_size = other._M_size;
            }
        }

        basic_allocator(std::basic_allocator<type> &&other) noexcept
            : _M_ptr(other._M_ptr), _M_size(other._M_size)
        {
            other._M_ptr = nullptr;
            other._M_size = 0;
        }

        std::basic_allocator<type> &operator=(const std::basic_allocator<type> &other)
        {
            if (this != &other)
            {
                if (_M_ptr)
                {
                    delete[] _M_ptr;
                    _M_ptr = nullptr;
                    _M_size = 0;
                }

                if (other._M_ptr && other._M_size > 0)
                {
                    _M_ptr = new type[other._M_size];
                    std::copy(other._M_ptr, other._M_ptr + other._M_size, _M_ptr);
                    _M_size = other._M_size;
                }
            }
            return *this;
        }
        std::basic_allocator<type> &operator=(std::basic_allocator<type> &&other)
        {
            if (this != &other)
            {
                if (_M_ptr)
                {
                    delete[] _M_ptr;
                    _M_ptr = nullptr;
                    _M_size = 0;
                }
                _M_ptr = other._M_ptr;
                _M_size = other._M_size;
                other._M_ptr = nullptr;
                other._M_size = 0;
            }
            return *this;
        }
        type &operator[](size_t index) { return _M_ptr[index]; }
        const type &operator[](size_t index) const { return _M_ptr[index]; }
        type &at(size_t index)
        {
            if (index >= _M_size)
                throw std::out_of_range("index out of range: " + std::to_string(index));
            return _M_ptr[index];
        }

        const type &at(size_t index) const
        {
            if (index >= _M_size)
                throw std::out_of_range("index out of range: " + std::to_string(index));
            return _M_ptr[index];
        }

        type *begin() { return _M_ptr; }
        const type *begin() const { return _M_ptr; }
        type *end() { return _M_ptr + _M_size; }
        const type *end() const { return _M_ptr + _M_size; }

        // Capacity
        size_t size() const { return _M_size; }
        bool empty() const { return _M_size == 0; }

        ~basic_allocator()
        {
            if (_M_ptr)
                delete[] _M_ptr;
            _M_ptr = nullptr;
            _M_size = 0;
        }
    };
    template <typename type>
    basic_allocator<type>::basic_allocator(const std::initializer_list<type> &list)
        : _M_ptr(nullptr), _M_size(0)
    {
        if (list.size())
        {
            _M_ptr = new type[list.size()];
            std::copy(list.begin(), list.end(), _M_ptr);
            _M_size = list.size();
        }
    }
    template <typename type>
    basic_allocator<type>::basic_allocator(const std::vector<type> &list)
        : _M_ptr(nullptr), _M_size(0)
    {
        if (!list.empty())
        {
            _M_ptr = new type[list.size()];
            std::copy(list.begin(), list.end(), _M_ptr);
            _M_size = list.size();
        }
    }
} // namespace std
//<--...

#endif