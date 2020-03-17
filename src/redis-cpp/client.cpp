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
    impl(std::string_view host, std::string_view port,
         on_data_func on_data)
        : on_data_{std::move(on_data)}
    {
        boost::asio::ip::tcp::resolver resolver{io_};
        resolver.resolve(host, port);
    }

    ~impl()
    {
    }

    void post(std::string_view data)
    {
        (void)data;
    }

private:
    on_data_func on_data_;
    boost::asio::io_context io_;
};

client::client(std::string_view host, std::string_view port, on_data_func on_data)
    : impl_{std::make_unique<impl>(std::move(host), std::move(port), std::move(on_data))}
{
}

client::~client()
{
}

void client::post(std::string_view data)
{
    impl_->post(std::move(data));
}
}   // namespace rediscpp
