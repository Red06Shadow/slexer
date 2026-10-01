#ifndef STACKBUFFER
#define STACKBUFFER
#include <utilities/memory.hpp>
#include <slexer/buffer/buffer.hpp>

namespace slexer
{
    template <typename charT>
    class basic_stackbuffer
    {
    private:
        struct _S_node
        {
            slexer::basic_buffer<charT> _M_buffer;
            std::shared_ptr<_S_node> _M_back;
            _S_node(size_t size, std::shared_ptr<_S_node>&& back) : _M_buffer(size), _M_back(std::move(back)) {}
            _S_node(const std::basic_string<charT>& str, std::shared_ptr<_S_node>&& back) : _M_buffer(str), _M_back(std::move(back)) {}
        };
        std::shared_ptr<_S_node> _M_iterator;
        size_t _M_size;

    public:
        basic_stackbuffer() : _M_iterator(), _M_size(0) {}
        void push(size_t size)
        {
            if (_M_iterator.get() == nullptr)
                _M_iterator = std::make_shared<_S_node>(_S_node(size, nullptr));
            else
                _M_iterator = std::make_shared<_S_node>(size, std::move(_M_iterator));
            _M_size++;
        }
        void push(const std::basic_string<charT>& str)
        {
            if (_M_iterator.get() == nullptr)
                _M_iterator = std::make_shared<_S_node>(_S_node(str, nullptr));
            else
                _M_iterator = std::make_shared<_S_node>(size, std::move(_M_iterator));
            _M_size++;
        }
    
        inline slexer::basic_buffer<charT> &top()
        {
            if (_M_size == 0)
                throw std::runtime_error("in: basic_stackbuffer::top(); this stack is empty.");
            return _M_iterator.get()->_M_buffer;
        }
        inline const slexer::basic_buffer<charT> &top() const
        {
            if (_M_size == 0)
                throw std::runtime_error("in: basic_stackbuffer::top(); this stack is empty.");
            return _M_iterator.get()->_M_buffer;
        }
        void pop()
        {
            if (_M_iterator.get() == nullptr)
                throw std::runtime_error("in: basic_stackbuffer::pop(); this stack is empty.");
            std::shared_ptr provitional = std::move(_M_iterator->_M_back);
            _M_iterator->_M_buffer.~basic_buffer();
            _M_iterator.reset();
            _M_iterator = std::move(provitional);
            _M_size--;
        }
        inline size_t size() const { return _M_size; }
        ~basic_stackbuffer() {
            
        }
    };
} // namespace slexer

#endif