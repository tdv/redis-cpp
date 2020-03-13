//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef __REDISCPP_RESP_DETAIL_OVERLOADED_H__
#define __REDISCPP_RESP_DETAIL_OVERLOADED_H__

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

#endif  // !__REDISCPP_RESP_DETAIL_MARKER_H__
