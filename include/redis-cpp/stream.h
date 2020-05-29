//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef REDISCPP_STREAM_H_
#define REDISCPP_STREAM_H_

#ifndef REDISCPP_PURE_CORE

// STD
#include <iosfwd>
#include <memory>
#include <string_view>

// REDIS-CPP
#include <redis-cpp/detail/config.h>

namespace rediscpp
{

[[nodiscard]]
std::shared_ptr<std::iostream> make_stream(
        char* host, int port);

}   // namespace rediscpp

#ifdef REDISCPP_HEADER_ONLY
#include <redis-cpp/detail/stream.hpp>
#endif  // !REDISCPP_HEADER_ONLY

#endif  // !REDISCPP_PURE_CORE

#endif  // !REDISCPP_STREAM_H_
