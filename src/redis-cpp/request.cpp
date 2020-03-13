//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

// REDIS-CPP
#include <redis-cpp/request.h>

namespace rediscpp
{

request::request()
{
}

void request::pack_args(string_list const &args)
{
    //resp::serialization::array{args};
}

}   // namespace rediscpp
