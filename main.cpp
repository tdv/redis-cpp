// STD
#include <iostream>
#include <sstream>

#include "redis-cpp/resp/serialization.h"
#include "redis-cpp/resp/deserialization.h"

#include "redis-cpp/client.h"
#include "redis-cpp/request.h"

int main()
{
    try
    {
        rediscpp::client client{"127.0.0.1", "6379"};
        rediscpp::request request{"set", "my_key", "any value that is coupled with my_key", "ex", "60"};

        std::cout << request.data() << std::endl;
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
