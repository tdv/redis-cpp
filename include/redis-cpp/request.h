//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef __REDISCPP_REQUEST_H__
#define __REDISCPP_REQUEST_H__

// STD
#include <sstream>
#include <string_view>
#include <forward_list>

namespace rediscpp
{

class request final
{
public:
    template <typename ... TArgs>
    request(std::string_view name, TArgs && ... args)
        : request()
    {
        static_assert(
                (std::is_convertible_v<TArgs, std::string_view> && ... && true),
                "[rediscpp::request] All arguments of have to be convertable into std::string_view"
            );

        pack_args({name, std::string_view{args} ... });
    }

    request(request const &) = delete;
    request& operator = (request const &) = delete;

    request(request &&) = default;
    request& operator = (request &&) = default;

    // TODO: change it for std::string_view
    std::string const data() const;

private:
    using string_list = std::forward_list<std::string_view>;

    std::stringstream stream_;

    request();

    void pack_args(string_list const &args);
};

}   // namespace rediscpp

#endif  // !__REDISCPP_REQUEST_H__
