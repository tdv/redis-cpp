//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

#ifndef REDISCPP_RESP_DETAIL_VISITOR_H_
#define REDISCPP_RESP_DETAIL_VISITOR_H_

// STD
#include <stdexcept>
#include <utility>

// BOOST
#include <boost/variant/static_visitor.hpp>

// REDIS-CPP
#include <redis-cpp/detail/config.h>

namespace rediscpp
{
inline namespace resp
{
    namespace detail
    {

    template <typename ...>
    class functor_adapter;

    template <typename T, typename ... Tail>
    class functor_adapter<T, Tail ...>
        : public T
        , public functor_adapter<Tail ...>
    {
    public:
        functor_adapter(T &&arg, Tail && ... args)
            : T{std::forward<T>(arg)}
            , functor_adapter<Tail ...>(std::forward<Tail>(args) ...)
        {
        }

        template <typename Y>
        void operator () (Y &&arg) const
        {
            this->execute(std::forward<Y>(arg));
        }

    protected:
        using functor_adapter<Tail ...>::execute;

        template <typename Y>
        auto execute(Y &&arg) const
                -> decltype (T::operator () (std::forward<Y>(arg)))
        {
            return T::operator () (std::forward<Y>(arg));
        }
    };

    template <>
    class functor_adapter<>
        : public boost::static_visitor<void>
    {
    protected:
        void execute (...) const
        {
            throw std::bad_cast{};
        }
    };

    template <typename ... T>
    struct visitor
        : public functor_adapter<T ...>
    {
        using functor_adapter<T ...>::functor_adapter;
    };

    template <typename ... T>
    visitor<T ...> make_visitor(T && ... args)
    {
        return {std::forward<T>(args) ... };
    }

    }   // namespace detail
}   // namespace resp
}   // namespace rediscpp

#endif  // !REDISCPP_RESP_DETAIL_VISITOR_H_
