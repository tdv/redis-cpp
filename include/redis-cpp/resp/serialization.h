//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

#ifndef REDISCPP_RESP_SERIALIZATION_H_
#define REDISCPP_RESP_SERIALIZATION_H_

// STD
#include <cstdint>
#include <forward_list>
#include <ostream>
#include <string>
#include <type_traits>
#include <tuple>
#include <utility>

// REDIS-CPP
#include <redis-cpp/detail/config.h>
#include <redis-cpp/resp/detail/marker.h>
#include <redis-cpp/resp/detail/visitor.h>

namespace rediscpp
{
inline namespace resp
{
namespace serialization
{

template <typename T>
void put(std::ostream &stream, T &&value)
{
    value.put(stream);
}

class simple_string final
{
public:
    simple_string(std::string const &value) noexcept
        : value_{std::move(value)}
    {
    }

    void put(std::ostream &stream) const
    {
        stream << detail::marker::simple_string
               << value_
               << detail::marker::cr
               << detail::marker::lf;
    }

private:
    std::string value_;
};

class error_message final
{
public:
    error_message(std::string const &value) noexcept
        : value_{std::move(value)}
    {
    }

    void put(std::ostream &stream) const
    {
        stream << detail::marker::error_message
               << value_
               << detail::marker::cr
               << detail::marker::lf;
    }

private:
    std::string value_;
};

template <typename T>
class integer final
{
public:
    static_assert(
            std::is_integral<typename std::decay<T>::type>::value,
            "The class \"rediscpp::resp::serialization::integer\" "
            "has to be created only from an integral type."
        );

    integer(T && value) noexcept
        : value_{std::move(value)}
    {
    }

    void put(std::ostream &stream) const
    {
        stream << detail::marker::integer
               << static_cast<std::int64_t>(value_)
               << detail::marker::cr
               << detail::marker::lf;
    }

private:
    T value_;
};

class bulk_string final
{
public:
    bulk_string() noexcept
        : value_{}
    {
    }

    bulk_string(std::string const &value) noexcept
        : value_{std::move(value)}
    {
    }

    void put(std::ostream &stream) const
    {
        if (value_.data())
        {
            stream << detail::marker::bulk_string
                   << value_.length()
                   << detail::marker::cr
                   << detail::marker::lf
                   << value_
                   << detail::marker::cr
                   << detail::marker::lf;
        }
        else
        {
            stream << detail::marker::bulk_string
                   << -1
                   << detail::marker::cr
                   << detail::marker::lf;
        }
    }

private:
    std::string value_;
};

class binary_data final
{
public:
    binary_data(void const *data, std::size_t length) noexcept
        : data_{data}
        , length_{length}
    {
    }

    void put(std::ostream &stream) const
    {
        if (data_)
        {
            stream << detail::marker::bulk_string
                   << length_
                   << detail::marker::cr
                   << detail::marker::lf;

            stream.write(static_cast<std::ostream::char_type const *>(data_),
                    static_cast<std::streamsize>(length_));

            stream << detail::marker::cr
                   << detail::marker::lf;
        }
        else
        {
            stream << detail::marker::bulk_string
                   << -1
                   << detail::marker::cr
                   << detail::marker::lf;
        }
    }

private:
    void const *data_;
    std::size_t length_;
};

class null final
{
public:
    void put(std::ostream &stream) const
    {
        serialization::put(stream, bulk_string{});
    }
};

template <typename ... T>
class array final
{
public:
    array(T && ... values) noexcept
        : values_{std::make_tuple(std::forward<T>(values) ... )}
    {
    }

    void put(std::ostream &stream) const
    {
        stream << detail::marker::array
               << std::tuple_size<tuple_type>::value
               << detail::marker::cr
               << detail::marker::lf;

        print<std::tuple_size<tuple_type>::value>(stream);
    }

private:
    using tuple_type = std::tuple<typename std::decay<T>::type ... >;
    std::tuple<typename std::decay<T>::type ... > values_;

    template <std::size_t I>
    typename std::enable_if<I, void>::type print(std::ostream &stream) const
    {
        print<I - 1>(stream);
        serialization::put(stream, std::get<I - 1>(values_));
    };

    template <std::size_t I>
    typename std::enable_if<!I, void>::type print(std::ostream &) const
    {
    };
};

template <typename ... T>
array<T ...> make_array(T && ... args)
{
    return {std::forward<T>(args) ...};
}

template <>
class array<null> final
{
public:
    array(null const &)
    {
    }


    void put(std::ostream &stream) const
    {
        stream << detail::marker::array
               << -1
               << detail::marker::cr
               << detail::marker::lf;
    }
};

}   // namespace serialization
}   // namespace resp
}   // namespace rediscpp

#endif  // !REDISCPP_RESP_SERIALIZATION_H_
