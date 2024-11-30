#ifndef REDISCPP_RESP_DETAIL_VALUE_PTR_H_
#define REDISCPP_RESP_DETAIL_VALUE_PTR_H_

#include <memory>

namespace rediscpp
{
inline namespace resp
{
namespace detail
{
template<class T, class D = std::default_delete<T>, class = std::enable_if_t<!std::is_array_v<T> && !std::is_polymorphic_v<T>>>
class value_ptr: public std::unique_ptr<T, D>
{
    using parent_t = std::unique_ptr<T, D>;
public:
    template<class... Args, class = std::enable_if_t<std::is_constructible_v<parent_t, Args...>>>
    value_ptr(Args&&... args)
        : parent_t(std::forward<Args>(args)...)
    {}
    
    value_ptr(const value_ptr& other)
        : parent_t(other ? std::make_unique<T>(*other) : nullptr)
    {}
    value_ptr& operator=(const value_ptr& other)
    {
        if(other)
        {
            if(!*this)
            {
                parent_t::operator=(std::make_unique<T>(*other));
            }
            else
            {
                **this = *other;
            }
        }
        else
        {
            parent_t::reset();
        }
        return *this;
    }
    value_ptr& operator=(value_ptr&& other)
    {
        parent_t::operator=(std::move(other));
        return *this;
    }
    value_ptr& operator=(std::nullptr_t)
    {
        parent_t::operator=(nullptr);
        return *this;
    }
};

template<class T, class... Args>
value_ptr<T> make_value_ptr(Args&& ...args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

}   // namespace detail
}   // namespace resp
}   // namespace rediscpp

#endif  // !REDISCPP_RESP_DETAIL_VALUE_PTR_H_
