//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

#ifndef REDISCPP_RESP_DESERIALIZATION_H_
#define REDISCPP_RESP_DESERIALIZATION_H_

// STD
#include <cstdint>
#include <istream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <map>
#include <set>
#include <optional>

// BOOST
#include <boost/mp11.hpp>

// REDIS-CPP
#include <redis-cpp/detail/config.h>
#include <redis-cpp/resp/detail/marker.h>
#include <redis-cpp/resp/detail/value_ptr.h>

namespace rediscpp
{
inline namespace resp
{
namespace deserialization
{
[[nodiscard]]
inline char get_mark(std::istream &stream)
{
    auto mark = stream.get();
    switch (mark)
    {
    case detail::marker::simple_string :
    case detail::marker::error_message :
    case detail::marker::integer :
    case detail::marker::bulk_string :
    case detail::marker::array :
    case detail::marker::null :
    case detail::marker::boolean :
    case detail::marker::double_float :
    case detail::marker::big_number :
    case detail::marker::bulk_error :
    case detail::marker::verbatim_string :
    case detail::marker::map :
    case detail::marker::attributes_alt :
    case detail::marker::attributes :
    case detail::marker::set :
    case detail::marker::push :
        return char(mark);
    default:
        break;
    }
    throw std::invalid_argument{
            "[rediscpp::resp::deserialization::get_mark] "
            "Bad input format."
        };
}

template <typename T>
[[nodiscard]]
T get(std::istream &stream)
{
    return {stream};
}


class mapped_data final
{
public:
    struct items_type;

    mapped_data(std::istream &stream);
    mapped_data(const mapped_data &other) noexcept;
    mapped_data(mapped_data &&other) noexcept;
    mapped_data& operator=(const mapped_data &other) noexcept;
    mapped_data& operator=(mapped_data &&other) noexcept;
    ~mapped_data() noexcept;

    [[nodiscard]]
    std::size_t size() const noexcept;

    [[nodiscard]]
    items_type const& get() const noexcept;

private:
    [[nodiscard]]
    items_type* get_map() noexcept
    {
        return reinterpret_cast<items_type*>(map_buffer);
    }
    [[nodiscard]]
    items_type const* get_map() const noexcept
    {
        return reinterpret_cast<const items_type*>(map_buffer);
    }

    alignas(std::map<int, int, std::less<>>) std::byte map_buffer[sizeof(std::map<int, int, std::less<>>)];
};

class attributes final
{
public:
    static constexpr auto marker = detail::marker::attributes;
    using items_type = mapped_data::items_type;

    attributes(std::istream &stream)
        : items_(stream)
    {}

    [[nodiscard]]
    std::size_t size() const noexcept { return items_.size(); }

    [[nodiscard]]
    items_type const& get() const noexcept { return items_.get(); }
private:
    mapped_data items_;
};

template<class T>
auto operator<(const T& rhs, const T& lhs) -> decltype(rhs.get() < lhs.get())
{
    return rhs.get() < lhs.get();
}

struct set_attributes_fn
{
    template<class T>
    void operator()(T& value, detail::value_ptr<attributes> attrs) const
    {
        value.attributes_ = std::move(attrs);
    }

    template<class... Ts>
    void operator()(std::variant<Ts...>& variant, detail::value_ptr<attributes> attrs) const
    {
        std::visit([&](auto& value){(*this)(value, std::move(attrs));}, variant);
    }
};

constexpr set_attributes_fn set_attributes{};

class attributed
{
public:
    bool has_attributes() const
    {
        return bool(attributes_);
    }
    
    attributes& get_attributes() const
    {
        if(has_attributes())
        {
            return *attributes_;
        }
        throw std::bad_optional_access();
    }

    friend struct set_attributes_fn;

protected:
    detail::value_ptr<attributes> attributes_;
};

class simple_string final: private attributed
{
public:
    static constexpr auto marker = detail::marker::simple_string;
    simple_string(std::istream &stream)
    {
        std::getline(stream, value_);
        value_.pop_back(); // removing '\r' from string
    }

    [[nodiscard]]
    std::string_view get() const noexcept
    {
        return value_;
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    std::string value_;
};

class error_message final: private attributed
{
public:
    static constexpr auto marker = detail::marker::error_message;
    error_message(std::istream &stream)
    {
        std::getline(stream, value_);
        value_.pop_back(); // removing '\r' from string
    }

    [[nodiscard]]
    std::string_view get() const noexcept
    {
        return value_;
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    std::string value_;
};

class integer final: private attributed
{
public:
    static constexpr auto marker = detail::marker::integer;
    integer(std::istream &stream)
    {
        std::string string;
        std::getline(stream, string);
        value_ = std::stoll(string);
    }

    [[nodiscard]]
    std::int64_t get() const noexcept
    {
        return value_;
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    std::int64_t value_;
};

class binary_data final
{
public:
    binary_data(std::istream &stream)
    {
        std::string string;
        std::getline(stream, string);
        auto const length = std::stoll(string);
        if (length < 0)
        {
            is_null_ = true;
            return;
        }
        if (length > 0)
        {
            data_.resize(static_cast<typename buffer_type::size_type>(length));
            stream.read(&data_[0], length);
        }
        std::getline(stream, string);
    }

    [[nodiscard]]
    bool is_null() const noexcept
    {
        return is_null_;
    }

    [[nodiscard]]
    std::string_view get() const noexcept
    {
        return {std::data(data_), std::size(data_)};
    }

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return std::size(data_);
    }

    [[nodiscard]]
    char const* data() const noexcept
    {
        return std::data(data_);
    }

private:
    using buffer_type = std::vector<char>;
    bool is_null_ = false;
    buffer_type data_;
};

class bulk_string final: private attributed
{
public:
    static constexpr auto marker = detail::marker::bulk_string;
    bulk_string(std::istream &stream)
        : data_{stream}
    {
    }

    [[nodiscard]]
    bool is_null() const noexcept
    {
        return data_.is_null();
    }

    [[nodiscard]]
    std::string_view get() const noexcept
    {
        return data_.get();
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    binary_data data_;
};

class null final: private attributed
{
public:
    static constexpr auto marker = detail::marker::null;
    null(std::istream &stream)
    {
        std::string string;
        std::getline(stream, string);
    }

    void get() const noexcept
    {
    }

    static constexpr bool is_null() { return true; }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;
};

bool operator<(const null&, const null&)
{
    return false;
}

class boolean final: private attributed
{
public:
    static constexpr auto marker = detail::marker::boolean;
    boolean(std::istream &stream)
    {
        std::string string;
        std::getline(stream, string);
        switch (string.front())
        {
        case 't':
            value_ = true;
            break;
        case 'f':
            value_ = false;
            break;
        default:
            throw std::invalid_argument{
                    "[rediscpp::resp::deserialization::boolean] "
                    "Bad input format. Unsupported boolian value."
                };
        }
    }

    [[nodiscard]]
    bool get() const noexcept
    {
        return value_;
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    bool value_;
};

class double_float final: private attributed
{
public:
    static constexpr auto marker = detail::marker::double_float;
    double_float(std::istream &stream)
    {
        std::string string;
        std::getline(stream, string);
        value_ = std::stod(string);
    }

    [[nodiscard]]
    bool get() const noexcept
    {
        return value_;
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    double value_;
};

class big_number final: private attributed
{
public:
    static constexpr auto marker = detail::marker::big_number;
    big_number(std::istream &stream)
    {
        std::getline(stream, value_);
        value_.pop_back(); // removing '\r' from string
    }

    [[nodiscard]]
    std::string_view get() const noexcept
    {
        return value_;
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    std::string value_;
};

class bulk_error final: private attributed
{
public:
    static constexpr auto marker = detail::marker::bulk_error;
    bulk_error(std::istream &stream)
        : data_{stream}
    {
    }

    [[nodiscard]]
    std::string_view get() const noexcept
    {
        return data_.get();
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    binary_data data_;
};

class verbatim_string final: private attributed
{
public:
    static constexpr auto marker = detail::marker::verbatim_string;
    constexpr static size_t encoding_length = 3;

    verbatim_string(std::istream &stream)
    {
        std::string string;
        std::getline(stream, string);
        auto const length = std::stoll(string);
        data_.resize(static_cast<typename buffer_type::size_type>(length) + encoding_length);
        stream.read(&data_[0], encoding_length);
        stream.get();
        stream.read(&data_[encoding_length], length);
        std::getline(stream, string);
    }

    [[nodiscard]]
    std::string_view get() const noexcept
    {
        return {data(), size()};
    }
    
    [[nodiscard]]
    std::string_view get_encoding() const noexcept
    {
        return {std::data(data_), encoding_length};
    }

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return std::size(data_) - encoding_length;
    }

    [[nodiscard]]
    char const* data() const noexcept
    {
        return std::data(data_) + encoding_length;
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    using buffer_type = std::vector<char>;
    buffer_type data_;
};

class map final: private attributed
{
public:
    static constexpr auto marker = detail::marker::map;
    using items_type = mapped_data::items_type;

    map(std::istream &stream)
        : items_(stream)
    {}

    [[nodiscard]]
    std::size_t size() const noexcept { return items_.size(); }

    [[nodiscard]]
    items_type const& get() const noexcept { return items_.get(); }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    mapped_data items_;
};

class set final: private attributed
{
public:
    static constexpr auto marker = detail::marker::set;
    struct items_type;

    set(std::istream &stream);
    set(const set &other) noexcept;
    set(set &&other) noexcept;
    set& operator=(const set &other) noexcept;
    set& operator=(set &&other) noexcept;
    ~set() noexcept;

    [[nodiscard]]
    std::size_t size() const noexcept;

    [[nodiscard]]
    items_type const& get() const noexcept;

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    [[nodiscard]]
    items_type* get_set() noexcept
    {
        return reinterpret_cast<items_type*>(set_buffer);
    }
    [[nodiscard]]
    items_type const* get_set() const noexcept
    {
        return reinterpret_cast<const items_type*>(set_buffer);
    }

    alignas(std::set<int, std::less<>>) std::byte set_buffer[sizeof(std::set<int, std::less<>>)];
};

class push final: private attributed
{
public:
    static constexpr auto marker = detail::marker::push;
    struct items_type;

    push(std::istream &stream);
    push(const push &other) noexcept;
    push(push &&other) noexcept;
    push& operator=(const push &other) noexcept;
    push& operator=(push &&other) noexcept;
    ~push() noexcept;

    [[nodiscard]]
    std::size_t size() const noexcept;

    [[nodiscard]]
    items_type const& get() const noexcept;

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    [[nodiscard]]
    items_type* get_items() noexcept
    {
        return reinterpret_cast<items_type*>(vector_buffer);
    }
    [[nodiscard]]
    items_type const* get_items() const noexcept
    {
        return reinterpret_cast<const items_type*>(vector_buffer);
    }

    alignas(std::vector<int>) std::byte vector_buffer[sizeof(std::vector<int>)];
};

class array final: private attributed
{
public:
    static constexpr auto marker = detail::marker::array;
    using item_type = std::variant<
            simple_string,
            error_message,
            integer,
            bulk_string,
            array,
            null,
            boolean,
            double_float,
            big_number,
            bulk_error,
            verbatim_string,
            map,
            set,
            push
        >;

    using items_type = std::vector<item_type>;

    array(std::istream &stream);

    [[nodiscard]]
    bool is_null() const noexcept
    {
        return is_null_;
    }

    [[nodiscard]]
    std::size_t size() const noexcept
    {
        return std::size(items_);
    }

    [[nodiscard]]
    items_type const& get() const noexcept
    {
        return items_;
    }

    using attributed::has_attributes;
    using attributed::get_attributes;
    friend struct set_attributes_fn;

private:
    bool is_null_ = false;
    items_type items_;
};

constexpr char get_marker(size_t index)
{
    using namespace boost::mp11;
    return mp_with_index<mp_size<array::item_type>>(index, [](auto idx)
    {
        using idx_t = decltype(idx);
        return std::variant_alternative_t<idx_t::value, array::item_type>::marker;
    });
}

constexpr char get_marker(const array::item_type& item)
{
    return get_marker(item.index());
}

array::item_type parse_item(std::istream& stream)
{
    switch (get_mark(stream))
    {
    case detail::marker::simple_string :
        return simple_string{stream};
    case detail::marker::error_message :
        return error_message{stream};
    case detail::marker::integer :
        return integer{stream};
    case detail::marker::bulk_string :
        return bulk_string{stream};
    case detail::marker::array :
        return array{stream};
    case detail::marker::null :
        return null{stream};
    case detail::marker::boolean :
        return boolean{stream};
    case detail::marker::double_float :
        return double_float{stream};
    case detail::marker::big_number :
        return big_number{stream};
    case detail::marker::bulk_error :
        return bulk_error{stream};
    case detail::marker::verbatim_string :
        return verbatim_string{stream};
    case detail::marker::map :
        return map{stream};
    case detail::marker::attributes_alt :
    case detail::marker::attributes :
    {
        auto attrs = detail::make_value_ptr<attributes>(stream);
        auto value = parse_item(stream);
        set_attributes(value, std::move(attrs));
        return std::move(value);
    }
    case detail::marker::set :
        return set{stream};
    case detail::marker::push :
        return push{stream};
    default:
        break;
    }
    
    throw std::invalid_argument{
            "[rediscpp::resp::deserialization::parse_item] "
            "Bad input format. Unsupported value type."
        };
}

array::array(std::istream &stream)
{
    std::string string;
    std::getline(stream, string);
    auto count = std::stoll(string);
    if (count < 0)
    {
        is_null_ = true;
        return;
    }
    if (count < 1)
        return;
    items_.reserve(static_cast<typename items_type::size_type>(count));
    while (count--)
    {
        items_.emplace_back(deserialization::parse_item(stream));
    }
}

struct mapped_data::items_type: std::map<array::item_type, array::item_type, std::less<>>
{};

mapped_data::mapped_data(std::istream &stream)
{
    new(get_map()) items_type{};
    auto& items_ = *get_map();

    std::string string;
    std::getline(stream, string);
    auto count = std::stoll(string);

    while (count--)
    {
        auto key = parse_item(stream);
        items_.emplace(std::move(key), parse_item(stream));
    }
}

mapped_data::mapped_data(const mapped_data &other) noexcept
{
    new(get_map()) items_type{*other.get_map()};
}

mapped_data::mapped_data(mapped_data &&other) noexcept
{
    new(get_map()) items_type{std::move(*other.get_map())};
}

mapped_data& mapped_data::operator=(const mapped_data &other) noexcept
{
    *get_map() = *other.get_map();
    return *this;
}

mapped_data& mapped_data::operator=(mapped_data &&other) noexcept
{
    *get_map() = std::move(*other.get_map());
    return *this;
}

mapped_data::~mapped_data() noexcept
{
    get_map()->~map();
}

[[nodiscard]]
std::size_t mapped_data::size() const noexcept
{
    return std::size(get());
}

[[nodiscard]]
auto mapped_data::get() const noexcept -> items_type const&
{
    return *get_map();
}

struct set::items_type: std::set<array::item_type, std::less<>>
{};

set::set(std::istream &stream)
{
    new(get_set()) items_type{};
    auto& items_ = *get_set();

    std::string string;
    std::getline(stream, string);
    auto count = std::stoll(string);

    while (count--)
    {
        items_.emplace(parse_item(stream));
    }
}

set::set(const set &other) noexcept
{
    new(get_set()) items_type{*other.get_set()};
}

set::set(set &&other) noexcept
{
    new(get_set()) items_type{std::move(*other.get_set())};
}

set& set::operator=(const set &other) noexcept
{
    *get_set() = *other.get_set();
    return *this;
}

set& set::operator=(set &&other) noexcept
{
    *get_set() = std::move(*other.get_set());
    return *this;
}

set::~set() noexcept
{
    get_set()->~set();
}

[[nodiscard]]
std::size_t set::size() const noexcept
{
    return std::size(get());
}

[[nodiscard]]
auto set::get() const noexcept -> items_type const&
{
    return *get_set();
}

struct push::items_type: std::vector<array::item_type>
{};

push::push(std::istream &stream)
{
    new(get_items()) items_type{};

    auto& items_ = *get_items();

    std::string string;
    std::getline(stream, string);
    auto count = std::stoll(string);
    if (count < 1)
        return;
    items_.reserve(static_cast<typename items_type::size_type>(count));
    while (count--)
    {
        items_.emplace_back(parse_item(stream));
    }
}

push::push(const push &other) noexcept
{
    new(get_items()) items_type{*other.get_items()};
}

push::push(push &&other) noexcept
{
    new(get_items()) items_type{std::move(*other.get_items())};
}

push& push::operator=(const push &other) noexcept
{
    *get_items() = *other.get_items();
    return *this;
}

push& push::operator=(push &&other) noexcept
{
    *get_items() = std::move(*other.get_items());
    return *this;
}

push::~push() noexcept
{
    get_items()->~vector();
}

[[nodiscard]]
std::size_t push::size() const noexcept
{
    return std::size(get());
}

[[nodiscard]]
auto push::get() const noexcept -> items_type const&
{
    return *get_items();
}

}   // namespace deserialization
}   // namespace resp
}   // namespace rediscpp

#endif  // !REDISCPP_RESP_DESERIALIZATION_H_
