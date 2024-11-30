//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

#ifndef REDISCPP_VALUE_H_
#define REDISCPP_VALUE_H_

// STD
#include <algorithm>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <string_view>
#include <vector>

// REDIS-CPP
#include <redis-cpp/detail/config.h>
#include <redis-cpp/resp/deserialization.h>
#include <redis-cpp/resp/detail/overloaded.h>

namespace rediscpp
{

class value final
{
public:
    using item_type = resp::deserialization::array::item_type;

    value(std::istream &stream)
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
        case resp::detail::marker::null :
            item_ = std::make_unique<item_type>(resp::deserialization::null{stream});
            break;
        case resp::detail::marker::boolean :
            item_ = std::make_unique<item_type>(resp::deserialization::boolean{stream});
            break;
        case resp::detail::marker::double_float :
            item_ = std::make_unique<item_type>(resp::deserialization::double_float{stream});
            break;
        case resp::detail::marker::big_number :
            item_ = std::make_unique<item_type>(resp::deserialization::big_number{stream});
            break;
        case resp::detail::marker::bulk_error :
            item_ = std::make_unique<item_type>(resp::deserialization::bulk_error{stream});
            break;
        case resp::detail::marker::verbatim_string :
            item_ = std::make_unique<item_type>(resp::deserialization::verbatim_string{stream});
            break;
        case resp::detail::marker::map :
            item_ = std::make_unique<item_type>(resp::deserialization::map{stream});
            break;
        case resp::detail::marker::attributes_alt :
        case resp::detail::marker::attributes :
        {
            auto attrs = resp::detail::make_value_ptr<resp::deserialization::attributes>(stream);
            item_ = std::make_unique<item_type>(resp::deserialization::parse_item(stream));
            resp::deserialization::set_attributes(*item_, std::move(attrs));
            break;
        }
        case resp::detail::marker::set :
            item_ = std::make_unique<item_type>(resp::deserialization::set{stream});
            break;
        case resp::detail::marker::push :
            item_ = std::make_unique<item_type>(resp::deserialization::push{stream});
            break;
        default :
            break;
        }
    }

    value(item_type const &item)
        : marker_{get_marker(item)}
        , item_{std::make_unique<item_type>(item)}
    {
    }

    value(item_type &&item)
        : marker_{get_marker(item)}
        , item_{std::make_unique<item_type>(std::move(item))}
    {
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return !item_.operator bool();
    }

    [[nodiscard]]
    item_type const& get() const
    {
        if (empty())
            throw std::runtime_error{"Empty value."};

        return *item_;
    }

    [[nodiscard]]
    bool is_simple_string() const noexcept
    {
        return marker_ == resp::detail::marker::simple_string;
    }

    [[nodiscard]]
    bool is_error_message() const noexcept
    {
        return marker_ == resp::detail::marker::error_message;
    }

    [[nodiscard]]
    bool is_bulk_string() const noexcept
    {
        return marker_ == resp::detail::marker::bulk_string;
    }

    [[nodiscard]]
    bool is_integer() const noexcept
    {
        return marker_ == resp::detail::marker::integer;
    }

    [[nodiscard]]
    bool is_array() const noexcept
    {
        return marker_ == resp::detail::marker::array;
    }

    [[nodiscard]]
    bool is_null() const noexcept
    {
        return marker_ == resp::detail::marker::null 
            || (item_ ? std::visit([](auto val){return is_null(&val);}, *item_) : false);
    }

    [[nodiscard]]
    bool is_boolean() const noexcept
    {
        return marker_ == resp::detail::marker::boolean;
    }

    [[nodiscard]]
    bool is_double() const noexcept
    {
        return marker_ == resp::detail::marker::double_float;
    }

    [[nodiscard]]
    bool is_big_number() const noexcept
    {
        return marker_ == resp::detail::marker::big_number;
    }

    [[nodiscard]]
    bool is_bulk_error() const noexcept
    {
        return marker_ == resp::detail::marker::bulk_error;
    }

    [[nodiscard]]
    bool is_verbatim_string() const noexcept
    {
        return marker_ == resp::detail::marker::verbatim_string;
    }

    [[nodiscard]]
    bool is_map() const noexcept
    {
        return marker_ == resp::detail::marker::map;
    }

    [[nodiscard]]
    bool is_set() const noexcept
    {
        return marker_ == resp::detail::marker::set;
    }

    [[nodiscard]]
    bool is_push() const noexcept
    {
        return marker_ == resp::detail::marker::push;
    }

    [[nodiscard]]
    bool is_string() const noexcept
    {
        return is_simple_string() || is_bulk_string() || is_verbatim_string();
    }

    [[nodiscard]]
    bool is_error() const noexcept
    {
        return is_error_message() || is_bulk_error();
    }

    [[nodiscard]]
    auto as_error_message() const
    {
        return get_value<std::string_view, resp::deserialization::error_message>();
    }

    [[nodiscard]]
    auto as_simple_string() const
    {
        return get_value<std::string_view, resp::deserialization::simple_string>();
    }

    [[nodiscard]]
    auto as_integer() const
    {
        return get_value<std::int64_t, resp::deserialization::integer>();
    }

    [[nodiscard]]
    auto as_bulk_string() const
    {
        return get_value<std::string_view, resp::deserialization::bulk_string>();
    }

    [[nodiscard]]
    auto as_boolean() const
    {
        return get_value<bool, resp::deserialization::boolean>();
    }

    [[nodiscard]]
    auto as_double() const
    {
        return get_value<double, resp::deserialization::double_float>();
    }

    [[nodiscard]]
    auto as_big_number() const
    {
        return get_value<std::string_view, resp::deserialization::big_number>();
    }

    [[nodiscard]]
    auto as_bulk_error() const
    {
        return get_value<std::string_view, resp::deserialization::bulk_error>();
    }

    [[nodiscard]]
    auto as_verbatim_string() const
    {
        return get_value<std::string_view, resp::deserialization::bulk_string>();
    }

    [[nodiscard]]
    auto& as_map() const
    {
        return get_value<const std::map<item_type, item_type, std::less<>>&, resp::deserialization::map>();
    }

    [[nodiscard]]
    auto& as_set() const
    {
        return get_value<const std::set<item_type, std::less<>>&, resp::deserialization::set>();
    }

    [[nodiscard]]
    auto& as_push() const
    {
        return get_value<const std::vector<item_type>&, resp::deserialization::push>();
    }

    [[nodiscard]]
    auto& as_array() const
    {
        return get_value<const std::vector<item_type>&, resp::deserialization::array>();
    }

    [[nodiscard]]
    auto as_string() const
    {
        return is_simple_string() ?
                as_simple_string() :
               is_bulk_string() ?
                as_bulk_string() :
                as_verbatim_string();
    }

    [[nodiscard]]
    auto as_error() const
    {
        return is_error_message() ?
                as_error_message() :
                as_bulk_error();
    }

    [[nodiscard]]
    auto& as_vector() const
    {
        return is_array() ?
                as_array() :
                as_push();
    }

    [[nodiscard]]
    auto as_string_array() const
    {
        return get_array<std::string>();
    }

    [[nodiscard]]
    auto as_integer_array() const
    {
        return get_array<std::int64_t>();
    }

    template <typename T>
    operator T () const
    {
        return as<T>();
    }

    template <typename T>
    [[nodiscard]]
    T as() const
    {
        if (empty())
            throw std::runtime_error{"Empty value."};
        if (is_error())
            throw std::runtime_error{std::string{as_error()}};
        return T{get_value<std::decay_t<T>>()};
    }

    bool has_attributes()
    {
        if (empty())
            throw std::runtime_error{"Empty value."};
        return std::visit([](auto& value){return value.has_attributes();}, *item_);
    }

    const deserialization::attributes& get_attributes()
    {
        if (empty())
            throw std::runtime_error{"Empty value."};
        return std::visit([](auto& value) -> decltype(auto) {return value.get_attributes();}, *item_);
    }

private:
    char marker_;
    std::unique_ptr<item_type> item_;

    template <typename T>
    std::enable_if_t<std::is_integral_v<T>, T>
    get_value() const
    {
        return static_cast<T>(as_integer());
    }

    template <typename T>
    std::enable_if_t<
            std::is_same_v<T, std::string_view> ||
            std::is_same_v<T, std::string>, std::string_view>
    get_value() const
    {
        return as_string();
    }

    template <typename T>
    static auto is_null(T *v) noexcept
            -> decltype(v->is_null())
    {
        return v->is_null();
    }

    static  bool is_null(...) noexcept
    {
        return false;
    }

    template <typename R, typename T>
    R get_value() const
    {
        return std::visit(resp::detail::overloaded{
                [] (auto const &) -> R
                { throw std::bad_cast{}; },
                [] (T const &val) -> R
                {
                    if (is_null(&val))
                        throw std::logic_error("You can't cast Null to a type.");
                    return val.get();
                }
            }, get());
    }

    template<class T>
    static constexpr auto holds_vector = boost::mp11::mp_similar<std::decay_t<decltype(std::declval<T>().get())>, std::vector<int>>::value;

    template <typename T>
    std::vector<T> get_array() const
    {
        std::vector<T> result;

        std::visit(resp::detail::overloaded{
                [] ([[maybe_unused]]auto const &val)
                    -> std::enable_if_t<!holds_vector<decltype(val)>>
                { throw std::bad_cast{}; },
                [&result] (auto const &val)
                    -> std::enable_if_t<holds_vector<decltype(val)>>
                {
                    if (is_null(&val))
                        throw std::logic_error("You can't cast Null to a type.");
                    auto const &array = val.get();
                    std::transform(std::begin(array), std::end(array),
                            std::back_inserter(result),
                            [] (auto const &i) { return value{i}.as<T>(); }
                        );
                }
            }, get());

        return result;
    }
};

}   // namespace rediscpp

#endif  // !REDISCPP_VALUE_H_
