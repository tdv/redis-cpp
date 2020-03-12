// STD
#include <iostream>
#include <sstream>

#include "redis-cpp/core/protocol/resp/out.h"
#include "redis-cpp/core/protocol/resp/in.h"

#include "redis-cpp/net/client.h"

template<typename ... T>
struct overloaded
    : public T ...
{
    using T::operator() ... ;
};

template<typename ... T>
overloaded(T ... ) -> overloaded<T ... >;

int main()
{
    try
    {
        rediscpp::net::client client{"127.0.0.1", "6379"};

        std::stringstream os;
        auto &stream = os;
        put(stream, rediscpp::core::protocol::resp::out::simple_string{"This is a simple string!"});
        put(stream, rediscpp::core::protocol::resp::out::error_message{"This is an error message!"});
        put(stream, rediscpp::core::protocol::resp::out::integer{100500});
        put(stream, rediscpp::core::protocol::resp::out::integer{-100500});
        put(stream, rediscpp::core::protocol::resp::out::integer{true});
        put(stream, rediscpp::core::protocol::resp::out::integer{false});
        put(stream, rediscpp::core::protocol::resp::out::bulk_string{"This is a bulk string!"});
        put(stream, rediscpp::core::protocol::resp::out::bulk_string{});
        put(stream, rediscpp::core::protocol::resp::out::binary_data{"Str1\0Str2", 10});
        put(stream, rediscpp::core::protocol::resp::out::binary_data{nullptr, 0});
        put(stream, rediscpp::core::protocol::resp::out::array{
                rediscpp::core::protocol::resp::out::simple_string{"String item 1"},
                rediscpp::core::protocol::resp::out::null{},
                rediscpp::core::protocol::resp::out::integer{100}
            });
        put(stream, rediscpp::core::protocol::resp::out::array{
                rediscpp::core::protocol::resp::out::simple_string{"String item 1"},
                rediscpp::core::protocol::resp::out::null{},
                rediscpp::core::protocol::resp::out::integer{100},
                rediscpp::core::protocol::resp::out::array{
                    rediscpp::core::protocol::resp::out::integer{500100}
                }
            });
        put(stream, rediscpp::core::protocol::resp::out::array{
                rediscpp::core::protocol::resp::out::simple_string{"String item 14"},
                rediscpp::core::protocol::resp::out::integer{100}
            });
        put(stream, rediscpp::core::protocol::resp::out::array{
                rediscpp::core::protocol::resp::out::simple_string{"String item 15"}
            });
        put(stream, rediscpp::core::protocol::resp::out::array{
            });
        put(stream, rediscpp::core::protocol::resp::out::array{
                rediscpp::core::protocol::resp::out::null{}
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
            auto const leader = rediscpp::core::protocol::resp::in::get_mark(is);
            std::cout << "Leader: " << leader << std::endl;
            switch (leader)
            {
            case rediscpp::core::protocol::resp::detail::marker::simple_string :
            {
                rediscpp::core::protocol::resp::in::simple_string value{is};
                std::cout << "SimpleString: " << value.get() << std::endl;
                break;
            }
            case rediscpp::core::protocol::resp::detail::marker::error_message :
            {
                rediscpp::core::protocol::resp::in::error_message value{is};
                std::cout << "ErrorMessage: " << value.get() << std::endl;
                break;
            }
            case rediscpp::core::protocol::resp::detail::marker::integer :
            {
                rediscpp::core::protocol::resp::in::integer value{is};
                std::cout << "Integer: " << value.get() << std::endl;
                break;
            }
            case rediscpp::core::protocol::resp::detail::marker::bulk_string :
            {
                rediscpp::core::protocol::resp::in::bulk_string value{is};
                std::cout << "BulkString. IsNull: " << value.is_null() << " Size: " << value.get().length()
                        << " Value: " << value.get() << std::endl;
                break;
            }
            case rediscpp::core::protocol::resp::detail::marker::array :
            {
                rediscpp::core::protocol::resp::in::array value{is};
                std::cout << "Array of " << value.size() << " items." << std::endl;

                auto print = [] (rediscpp::core::protocol::in::array::items_type const &a)
                {
                    for (auto const &i : a)
                    {
                        std::visit(overloaded{[] (auto && val)
                                {val.get();},
                                [] (rediscpp::core::protocol::in::simple_string const &val)
                                {std::cout << "SimpleString: " << val.get() << std::endl;},
                                [] (rediscpp::core::protocol::in::error_message const &val)
                                {std::cout << "ErrorMessage: " << val.get() << std::endl;},
                                [] (rediscpp::core::protocol::in::integer const &val)
                                {std::cout << "Integer: " << val.get() << std::endl;},
                                [] (rediscpp::core::protocol::in::bulk_string const &val)
                                {std::cout << "BuilkString: " << val.get() << std::endl;},
                                [] (rediscpp::core::protocol::in::array const &val)
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
