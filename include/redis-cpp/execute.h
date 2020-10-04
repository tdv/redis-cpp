//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

#ifndef REDISCPP_EXECUTE_H_
#define REDISCPP_EXECUTE_H_

// STD
#include <iosfwd>
#include <string>
#include <type_traits>

// REDIS-CPP
#include <redis-cpp/detail/config.h>
#include <redis-cpp/resp/serialization.h>
#include <redis-cpp/value.h>

namespace rediscpp
{

template <typename ... TArgs>
inline void execute_no_flush(std::ostream &stream, std::string const &name, TArgs && ... args)
{
    put(stream, resp::serialization::make_array(
            resp::serialization::bulk_string{std::move(name)},
            resp::serialization::bulk_string{std::string{args}} ...
        ));
}

template <typename ... TArgs>
[[nodiscard]]
inline value execute(std::iostream &stream, std::string const &name, TArgs && ... args)
{
    execute_no_flush(stream, std::move(name), std::forward<TArgs>(args) ... );
    std::flush(stream);
    return {stream};
}

}   // namespace rediscpp

#endif  // !REDISCPP_EXECUTE_H__
