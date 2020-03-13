//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

// STD
#include <utility>

// BOOST
#include <boost/asio.hpp>

// REDIS-CPP
#include <redis-cpp/client.h>

namespace rediscpp
{

class client::impl final
{
public:
    impl(std::string_view host, std::string_view port)
    {
        boost::asio::ip::tcp::resolver resolver{io_};
        resolver.resolve(host, port);
    }

    ~impl()
    {
    }

    std::istream& get_istream()
    {
        return istream_;
    }

    std::ostream& get_ostream()
    {
        return ostream_;
    }

private:
    boost::asio::io_context io_;

    boost::asio::streambuf istreambuf_;
    boost::asio::streambuf ostreambuf_;

    std::istream istream_{&istreambuf_};
    std::ostream ostream_{&ostreambuf_};
};

client::client(std::string_view host, std::string_view port)
    : impl_{std::make_unique<impl>(std::move(host), std::move(port))}
{
}

client::~client()
{
}

std::istream& client::get_istream()
{
    return impl_->get_istream();
}

std::ostream& client::get_ostream()
{
    return impl_->get_ostream();
}

}   // namespace rediscpp
