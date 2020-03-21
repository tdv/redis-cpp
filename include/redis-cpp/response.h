//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright (C) 2020 tdv
//-------------------------------------------------------------------

#ifndef __REDISCPP_RESPONSE_H__
#define __REDISCPP_RESPONSE_H__

// STD
#include <iosfwd>
#include <memory>
#include <string_view>

// REDIS-CPP
#include <redis-cpp/resp/serialization.h>
#include <redis-cpp/resp/deserialization.h>

namespace rediscpp
{

class response final
{
public:
    using item_type = resp::deserialization::array::item_type;

    response(std::iostream &stream)
        : marker_{resp::deserialization::get_mark(stream)}
    {
        switch (marker_)
        {
        case resp::detail::marker::simple_string :
            item_ = std::make_unique<item_type>(resp::deserialization::simple_string{stream});
            break;
        case resp::detail::marker::error_message :
            item_ = std::make_unique<item_type>(resp::deserialization::error_message{stream});
            break;
        case resp::detail::marker::integer :
            item_ = std::make_unique<item_type>(resp::deserialization::integer{stream});
            break;
        case resp::detail::marker::bulk_string :
            item_ = std::make_unique<item_type>(resp::deserialization::bulk_string{stream});
            break;
        case resp::detail::marker::array :
            item_ = std::make_unique<item_type>(resp::deserialization::array{stream});
            break;
        default :
            break;
        }
    }

    response(item_type const &item)
        : marker_{resp::detail::marker::array}
        , item_{std::make_unique<item_type>(item)}
    {
    }

    bool empty() const noexcept
    {
        return !item_.operator bool();
    }

    item_type const& get() const
    {
        if (empty())
            throw std::runtime_error{"Empty response."};

        return *item_;
    }

    bool is_simple_string() const noexcept
    {
        return marker_ == resp::detail::marker::simple_string;
    }

    bool is_error_message() const noexcept
    {
        return marker_ == resp::detail::marker::error_message;
    }

    bool is_bulk_string() const noexcept
    {
        return marker_ == resp::detail::marker::bulk_string;
    }

    bool is_integer() const noexcept
    {
        return marker_ == resp::detail::marker::integer;
    }

    bool is_array() const noexcept
    {
        return marker_ == resp::detail::marker::array;
    }

    bool is_string() const noexcept
    {
        return is_simple_string() || is_bulk_string();
    }

    auto as_error_message() const
    {
        return get_value<std::string_view, resp::deserialization::error_message>();
    }

    auto as_simple_string() const
    {
        return get_value<std::string_view, resp::deserialization::simple_string>();
    }

    auto as_integer() const
    {
        return get_value<std::int64_t, resp::deserialization::integer>();
    }

    auto as_bulk_string() const
    {
        return get_value<std::string_view, resp::deserialization::bulk_string>();
    }

    auto as_string() const
    {
        return is_simple_string() ?
                get_value<std::string_view, resp::deserialization::simple_string>() :
                get_value<std::string_view, resp::deserialization::bulk_string>();
    }

private:
    std::unique_ptr<item_type> item_;
    char marker_;

    template <typename R, typename T>
    R get_value() const
    {
        R result;
        std::visit(detail::overloaded{
                [] (auto const &)
                {
                    throw std::bad_cast{};
                },
                [&result] (T const &value)
                {
                    result = value.get();
                }
            }, get());

        return result;
    }
};

template <typename ... TArgs>
inline void execute_no_flush(std::iostream &stream, std::string_view name, TArgs && ... args)
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
    return response{stream};
}

}   // namespace rediscpp

#endif  // !__REDISCPP_RESPONSE_H__
