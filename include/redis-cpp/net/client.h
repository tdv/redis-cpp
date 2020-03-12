//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef __REDISCPP_NET_CLIENT_H__
#define __REDISCPP_NET_CLIENT_H__

// STD
#include <istream>
#include <ostream>
#include <string_view>
#include <memory>

namespace rediscpp::net
{

class client final
{
public:
    client(std::string_view host, std::string_view port);
    ~client();

    std::istream& get_istream();
    std::ostream& get_ostream();

private:
    class impl;
    std::unique_ptr<impl> impl_;
};

}   // namespace rediscpp::net

#endif  // !__REDISCPP_NET_CLIENT_H__
