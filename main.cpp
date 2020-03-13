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

        std::cout << request.get_data() << std::endl;

        return EXIT_SUCCESS;

        std::stringstream os;
        auto &stream = os;
        put(stream, rediscpp::resp::serialization::simple_string{"This is a simple string!"});
        put(stream, rediscpp::resp::serialization::error_message{"This is an error message!"});
        put(stream, rediscpp::resp::serialization::integer{100500});
        put(stream, rediscpp::resp::serialization::integer{-100500});
        put(stream, rediscpp::resp::serialization::integer{true});
        put(stream, rediscpp::resp::serialization::integer{false});
        put(stream, rediscpp::resp::serialization::bulk_string{"This is a bulk string!"});
        put(stream, rediscpp::resp::serialization::bulk_string{});
        put(stream, rediscpp::resp::serialization::binary_data{"Str1\0Str2", 10});
        put(stream, rediscpp::resp::serialization::binary_data{nullptr, 0});
        put(stream, rediscpp::resp::serialization::array{
                rediscpp::resp::serialization::simple_string{"String item 1"},
                rediscpp::resp::serialization::null{},
                rediscpp::resp::serialization::integer{100}
            });
        put(stream, rediscpp::resp::serialization::array{
                rediscpp::resp::serialization::simple_string{"String item 1"},
                rediscpp::resp::serialization::null{},
                rediscpp::resp::serialization::integer{100},
                rediscpp::resp::serialization::array{
                    rediscpp::resp::serialization::integer{500100}
                }
            });
        put(stream, rediscpp::resp::serialization::array{
                rediscpp::resp::serialization::simple_string{"String item 14"},
                rediscpp::resp::serialization::integer{100}
            });
        put(stream, rediscpp::resp::serialization::array{
                rediscpp::resp::serialization::simple_string{"String item 15"}
            });
        put(stream, rediscpp::resp::serialization::array{
            });
        put(stream, rediscpp::resp::serialization::array{
                rediscpp::resp::serialization::null{}
            });

        auto data = os.str();
        std::cout << "--------------- Source -------------------------" << std::endl;
        std::cout << data << std::endl;
        std::cout << "--------------- New ----------------------------" << std::endl;

        std::stringstream is{data};
        // TODO: set locale for \r\n
        for ( ; ; )
        {
            if (is.peek() == -1)
            {
                std::cout << "That is all !" << std::endl;
                break;
            }
            auto const leader = rediscpp::resp::deserialization::get_mark(is);
            std::cout << "Leader: " << leader << std::endl;
            switch (leader)
            {
            case rediscpp::resp::detail::marker::simple_string :
            {
                rediscpp::resp::deserialization::simple_string value{is};
                std::cout << "SimpleString: " << value.get() << std::endl;
                break;
            }
            case rediscpp::resp::detail::marker::error_message :
            {
                rediscpp::resp::deserialization::error_message value{is};
                std::cout << "ErrorMessage: " << value.get() << std::endl;
                break;
            }
            case rediscpp::resp::detail::marker::integer :
            {
                rediscpp::resp::deserialization::integer value{is};
                std::cout << "Integer: " << value.get() << std::endl;
                break;
            }
            case rediscpp::resp::detail::marker::bulk_string :
            {
                rediscpp::resp::deserialization::bulk_string value{is};
                std::cout << "BulkString. IsNull: " << value.is_null() << " Size: " << value.get().length()
                        << " Value: " << value.get() << std::endl;
                break;
            }
            case rediscpp::resp::detail::marker::array :
            {
                rediscpp::resp::deserialization::array value{is};
                std::cout << "Array of " << value.size() << " items." << std::endl;

                auto print = [] (rediscpp::resp::deserialization::array::items_type const &a)
                {
                    for (auto const &i : a)
                    {
                        std::visit(rediscpp::resp::detail::overloaded{[] (auto && val)
                                {val.get();},
                                [] (rediscpp::resp::deserialization::simple_string const &val)
                                {std::cout << "SimpleString: " << val.get() << std::endl;},
                                [] (rediscpp::resp::deserialization::error_message const &val)
                                {std::cout << "ErrorMessage: " << val.get() << std::endl;},
                                [] (rediscpp::resp::deserialization::integer const &val)
                                {std::cout << "Integer: " << val.get() << std::endl;},
                                [] (rediscpp::resp::deserialization::bulk_string const &val)
                                {std::cout << "BuilkString: " << val.get() << std::endl;},
                                [] (rediscpp::resp::deserialization::array const &val)
                                {
                                    std::cout << "Array. Items: " << val.size() << std::endl;
                                }
                            }, i);
                    }
                };
                print(value.get());
                break;
            }
            default:
                break;
            }
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
