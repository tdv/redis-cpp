//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef __REDISCPP_CORE_PROTOCOL_RESP_DETAIL_MARKER_H__
#define __REDISCPP_CORE_PROTOCOL_RESP_DETAIL_MARKER_H__

namespace rediscpp::core::protocol
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
}   // namespace rediscpp::core::protocol

#endif  // !__REDISCPP_CORE_PROTOCOL_RESP_DETAIL_MARKER_H__
