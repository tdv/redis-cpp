//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef __REDISCPP_STREAM_H__
#define __REDISCPP_STREAM_H__

// STD
#include <iosfwd>
#include <memory>
#include <string_view>

namespace rediscpp
{

std::shared_ptr<std::iostream> make_stream(
        std::string_view host, std::string_view port);

}   // namespace rediscpp

#endif  // !__REDISCPP_STREAM_H__
