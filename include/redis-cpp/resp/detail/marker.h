//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef REDISCPP_RESP_DETAIL_MARKER_H_
#define REDISCPP_RESP_DETAIL_MARKER_H_

// REDIS-CPP
#include <redis-cpp/detail/config.h>

namespace rediscpp
{
inline namespace resp
{
namespace detail::marker
{

constexpr auto simple_string = '+';
constexpr auto error_message = '-';
constexpr auto integer = ':';
constexpr auto bulk_string = '$';
constexpr auto array = '*';

constexpr auto cr = '\r';
constexpr auto lf = '\n';

}   // namespace detail::marker
}   // namespace resp
}   // namespace rediscpp

#endif  // !REDISCPP_RESP_DETAIL_MARKER_H_
