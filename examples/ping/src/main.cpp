// STD
#include <cstdlib>
#include <iostream>

#include "redis-cpp/stream.h"
#include "redis-cpp/execute.h"

int main()
{
    try
    {
        auto stream = rediscpp::make_stream("localhost", "6379");
        auto value = rediscpp::execute(*stream, "ping");
        std::cout << "Response: " << value.as<std::string>() << std::endl;
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
