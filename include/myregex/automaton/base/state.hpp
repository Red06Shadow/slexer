#ifndef MYREGEXSTATE
#define MYREGEXSTATE
//<--...
namespace myregex
{
    template <typename idT>
    class state
    {
    private:
        bool _M_valid;
        idT _M_id;

    public:
        state() : _M_valid(false), _M_id() {}
        state(idT id) : _M_valid(true), _M_id(id) {}
        inline const idT &get() const { return _M_id; }
        inline bool valid() const {return _M_valid; }
    };

} // namespace myregex
//<--...
#endif