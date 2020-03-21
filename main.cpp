// STD
#include <iostream>
#include <sstream>

#include "redis-cpp/resp/serialization.h"
#include "redis-cpp/resp/deserialization.h"

#include "redis-cpp/stream.h"
#include "redis-cpp/request.h"

int main()
{
    try
    {
        auto stream = rediscpp::make_stream("localhost", "6379");
        std::iostream &ios = *stream;
        put(ios, rediscpp::resp::serialization::array{
                rediscpp::resp::serialization::bulk_string{"set"},
                rediscpp::resp::serialization::bulk_string{"my_key"},
                rediscpp::resp::serialization::bulk_string{"my new value for my key"},
                rediscpp::resp::serialization::bulk_string{"ex"},
                rediscpp::resp::serialization::bulk_string{"60"}
            });

        std::flush(ios);

        char ch = 0;
        ios >> ch;
        switch (ch)
        {
        case '+' :
            {
            rediscpp::resp::deserialization::simple_string ss{ios};
            std::cout << "Response: " << ss.get() << std::endl;
            }
            break;
        case '-' :
            {
            rediscpp::resp::deserialization::error_message ss{ios};
            std::cout << "Error: " << ss.get() << std::endl;
            }
            break;
        default:
            throw std::runtime_error{"Unsupported leading simbol."};
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
