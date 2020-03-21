// STD
#include <iostream>
#include <sstream>

#include "redis-cpp/resp/serialization.h"
#include "redis-cpp/resp/deserialization.h"

#include "redis-cpp/stream.h"
#include "redis-cpp/value.h"

int main()
{
    try
    {
        auto stream = rediscpp::make_stream("localhost", "6379");

        auto value = rediscpp::execute(*stream, "set", "my_key", "value for my_key", "ex", "60");

        if (value.is_error_message())
            std::cerr << "Error: " << value.as_error_message() << std::endl;
        else if (value.is_string())
            std::cout << "Respojse: " << value.as_string() << std::endl;
        else if (value.is_integer())
            std::cout << "Response: " << value.as_integer() << std::endl;
        else if (value.is_array())
        {
            rediscpp::value val{value.get()};
            (void)val;
        }

    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
