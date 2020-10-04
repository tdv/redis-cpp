//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

// STD
#include <cstdlib>
#include <iostream>
#include <sstream>

// BOOST
#include <boost/variant/apply_visitor.hpp>

// REDIS-CPP
#include <redis-cpp/execute.h>

namespace resps = rediscpp::resp::serialization;
namespace respds = rediscpp::resp::deserialization;

std::string make_sample_data()
{
    std::ostringstream stream;

    put(stream, resps::make_array(
            resps::simple_string{"This is a simple string."},
            resps::error_message{"This is an error message."},
            resps::bulk_string{"This is a bulk string."},
            resps::integer<std::size_t>{100500},
            resps::make_array(
                resps::simple_string("This is a simple string in a nested array."),
                resps::bulk_string("This is a bulk string in a nested array.")
            )
        ));

    return stream.str();
}

void print_value(respds::array::item_type const &value, std::ostream &stream)
{
    boost::apply_visitor(rediscpp::resp::detail::make_visitor(
            [&stream] (respds::simple_string const &val)
            { stream << "Simple string: " << val.get() << std::endl; },
            [&stream] (respds::error_message const &val)
            { stream << "Error message: " << val.get() << std::endl; },
            [&stream] (respds::bulk_string const &val)
            { stream << "Bulk string: " << val.get() << std::endl; },
            [&stream] (respds::integer const &val)
            { stream << "Integer: " << val.get() << std::endl; },
            [&stream] (respds::array const &val)
            {
                stream << "----- Array -----" << std::endl;
                for (auto const &i : val.get())
                    print_value(i, stream);
                stream << "-----------------" << std::endl;
            }
        ), value);
}

void print_sample_data(std::istream &istream, std::ostream &ostream)
{
    rediscpp::value value{istream};
    print_value(value.get(), ostream);
}

int main()
{
    try
    {
        auto const data = make_sample_data();
        std::cout << "------------ Serialization ------------" << std::endl;
        std::cout << data << std::endl;

        std::cout << "------------ Deserialization ------------" << std::endl;
        std::istringstream stream{data};
        print_sample_data(stream, std::cout);
        std::cout << std::endl;
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
