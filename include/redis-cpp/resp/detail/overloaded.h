//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef REDISCPP_RESP_DETAIL_OVERLOADED_H_
#define REDISCPP_RESP_DETAIL_OVERLOADED_H_

// REDIS-CPP
#include <redis-cpp/detail/config.h>

namespace rediscpp
{
inline namespace resp
{
namespace detail
{

template<typename ... T>
struct overloaded
    : public T ...
{
    using T::operator() ... ;
};

template<typename ... T>
overloaded(T ... ) -> overloaded<T ... >;

}   // namespace detail
}   // namespace resp
}   // namespace rediscpp

#endif  // !REDISCPP_RESP_DETAIL_MARKER_H_
