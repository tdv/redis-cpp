//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

// REDIS-CPP
#include <redis-cpp/request.h>
#include <redis-cpp/resp/serialization.h>

namespace rediscpp
{

request::request()
{
}

std::string const request::data() const
{
    return {stream_.str()};
}

void request::pack_args(string_list const &args)
{
    //put(stream_, resp::serialization::array{args});
}

}   // namespace rediscpp
