//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef __REDISCPP_CLIENT_H__
#define __REDISCPP_CLIENT_H__

// STD
#include <deque>
#include <functional>
#include <memory>
#include <string_view>

namespace rediscpp
{

class client final
{
public:
    using buffer = std::deque<char>;
    using on_data_func = std::function<void (buffer)>;

    client(std::string_view host, std::string_view port,
           on_data_func on_data = [](buffer){});
    ~client();

    void post(std::string_view data);

private:
    class impl;
    std::unique_ptr<impl> impl_;
};

}   // namespace rediscpp

#endif  // !__REDISCPP_CLIENT_H__
