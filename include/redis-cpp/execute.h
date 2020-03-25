//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef REDISCPP_EXECUTE_H_
#define REDISCPP_EXECUTE_H_

// STD
#include <iosfwd>
#include <string_view>
#include <type_traits>

// REDIS-CPP
#include <redis-cpp/detail/config.h>
#include <redis-cpp/resp/serialization.h>
#include <redis-cpp/value.h>

namespace rediscpp
{

template <typename ... TArgs>
inline void execute_no_flush(std::ostream &stream, std::string_view name, TArgs && ... args)
{
    static_assert(
            (std::is_convertible_v<TArgs, std::string_view> && ... && true),
            "[rediscpp::execute] All arguments of have to be convertable into std::string_view"
        );

    put(stream, resp::serialization::array{
            resp::serialization::bulk_string{std::move(name)},
            resp::serialization::bulk_string{std::string_view{args}} ...
        });
}

template <typename ... TArgs>
inline auto execute(std::iostream &stream, std::string_view name, TArgs && ... args)
{
    execute_no_flush(stream, std::move(name), std::forward<TArgs>(args) ... );
    std::flush(stream);
    return value{stream};
}

}   // namespace rediscpp

#endif  // !REDISCPP_EXECUTE_H__
