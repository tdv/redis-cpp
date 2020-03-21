// STD
#include <iostream>
#include <sstream>

#include "redis-cpp/resp/serialization.h"
#include "redis-cpp/resp/deserialization.h"

#include "redis-cpp/stream.h"
#include "redis-cpp/response.h"

int main()
{
    try
    {
        auto stream = rediscpp::make_stream("localhost", "6379");

        auto response = rediscpp::execute(*stream, "set", "my_key", "value for my_key", "ex", "60");

        if (response.is_error_message())
            std::cerr << "Error: " << response.as_error_message() << std::endl;
        else if (response.is_string())
            std::cout << "Response: " << response.as_string() << std::endl;
        else if (response.is_integer())
            std::cout << "Response: " << response.as_integer() << std::endl;
        else if (response.is_array())
        {
            rediscpp::response resp{response.get()};
            (void)resp;
        }

    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
