# This is a backport in c++11 of redis-cpp in c++17  
Switch to master branch in order to use main version in c++17, but if you need to use c++11, read below and use that. There are all examples have adapted to c++11.  

# redis-cpp - lightweight C++ client library for Redis
redis-cpp is a library in C++17 for executing Redis [commands](https://redis.io/commands) with support of the pipelines and publish / subscribe pattern. Moreover, you can extend the library by your own stream implementation to communicate with Redis. Alse, you can use it like RESP serializer (pure core).  
You can know only a couple functions to start to work with Rides through the library.  
```cpp
// Connect to server
auto stream = rediscpp::make_stream("localhost", "6379");
// Execute command
std::cout << rediscpp::execute(*stream, "ping").as<std::string>() << std::endl;
```
And you will dive deeper if you feel the necessity.  

# Version
1.0.0  

# Features
- easy way to access Redis
- pipelines
- publish / subscribe
- extensible transport
- header-only library if it's necessary
- minimal dependencies
- various levels of usage  


# License
Distributed under the MIT License

# Compiler and OS
This has compiled and tested within gcc 9.2.1 on Ubuntu 19.10.  
You might try other compiler or OS.  

**NOTE**  
All code is a cross-platform code.  

# Dependencies
- Boost

# Build and install
## Build library
```bash
git clone -b c++11 https://github.com/tdv/redis-cpp.git  
cd redis-cpp
mkdir build  
cd build  
cmake ..  
make  
make install  
```
You can use CMAKE_INSTALL_PREFIX to select the particular installation directory  
Moreover, you can use cmake options to configure library like header-only library.  
Instead of cmake options, you can define REDISCPP_HEADER_ONLY and use the library like header-only library without any cmake file.  

**NOTE**  
redis-cpp has a build options  
- Header-only  

Use cmake -D with REDISCPP_HEADER_ONLY.  

If you need to use the library like header-only library, you can copy the folder redis-cpp from include/redis-cpp in your project and define the macro REDISCPP_HEADER_ONLY before including redis-cpp headers according to the code below
```cpp
#define REDISCPP_HEADER_ONLY
#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

// Include something else
```

## Build examples
```bash
cd examples/{example_project}
mkdir build  
cd build  
cmake ..  
make  
```

# Examples

**NOTE**  
Look at the redis-docker folder to get all what you need to start testing redis-cpp. There are files to build and run Redis server in Docker.  

## Ping
[Source code](https://github.com/tdv/redis-cpp/tree/c%2B%2B11/examples/ping)  
**Description**  
The "Ping" example demonstrates how to execute a Redis command.  

```cpp
// STD
#include <cstdlib>
#include <iostream>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

int main()
{
    try
    {
        auto stream = rediscpp::make_stream("localhost", "6379");
        auto response = rediscpp::execute(*stream, "ping");
        std::cout << response.as<std::string>() << std::endl;
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```


## Set and Get data
[Source code](https://github.com/tdv/redis-cpp/tree/c%2B%2B11/examples/setget)  
**Description**  
The example demonstrates how to set and get a value.  

```cpp
// STD
#include <cstdlib>
#include <iostream>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

int main()
{
    try
    {
        auto stream = rediscpp::make_stream("localhost", "6379");

        auto const key = "my_key";

        auto response = rediscpp::execute(*stream, "set",
                key, "Some value for 'my_key'", "ex", "60");

        std::cout << "Set key '" << key << "': " << response.as<std::string>() << std::endl;

        response = rediscpp::execute(*stream, "get", key);
        std::cout << "Get key '" << key << "': " << response.as<std::string>() << std::endl;
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

## Pipeline
[Source code](https://github.com/tdv/redis-cpp/tree/c%2B%2B11/examples/pipeline)  
**Description**  
It's a more complicated example which demonstrates how to use a pipeline within Redis to reach a better performance.  

```cpp
// STD
#include <cstdlib>
#include <iostream>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

int main()
{
    try
    {
        auto stream = rediscpp::make_stream("localhost", "6379");

        int const N = 10;
        auto const key_pref = "my_key_";

        // Executing command 'SET' N times without getting any response
        for (int i = 0 ; i < N ; ++i)
        {
            auto const item = std::to_string(i);
            rediscpp::execute_no_flush(*stream,
                "set", key_pref + item, item , "ex", "60");
        }

        // Flush all
        std::flush(*stream);

        // Getting response for each sended 'SET' request
        for (int i = 0 ; i < N ; ++i)
        {
            rediscpp::value value{*stream};
            std::cout << "Set " << key_pref << i << ": "
                      << value.as<std::string>() << std::endl;
        }

        // Executing command 'GET' N times without getting any response
        for (int i = 0 ; i < N ; ++i)
        {
            rediscpp::execute_no_flush(*stream, "get",
                key_pref + std::to_string(i));
        }

        // Flush all
        std::flush(*stream);

        // Getting response for each sended 'GET' request
        for (int i = 0 ; i < N ; ++i)
        {
            rediscpp::value value{*stream};
            std::cout << "Get " << key_pref << i << ": "
                      << value.as<std::string>() << std::endl;
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

## Resp
[Source code](https://github.com/tdv/redis-cpp/tree/c%2B%2B11/examples/resp)  
**Description**  
The "Resp" example demonstrates a basic RESP serialization within redis-cpp without communication with Redis server. It's only an exampe to show how to use RESP serialization with redis-cpp librarry.  
```cpp
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
```

## Publish / Subscribe
[Source code](https://github.com/tdv/redis-cpp/tree/c%2B%2B11/examples/pubsub)  
**Description**  
This is a more complicated example within redis-cpp which demonstrates how to publicate messages and make subscription onto a queue. In the example a publisher and subscriber locate in one process simultaniously, each one has its own stream to communicate with Redis. Usually, in real projects the publisher and subscriber are not located in one process.  

```cpp
// STD
#include <cstdlib>
#include <iostream>
#include <thread>

// BOOST
#include <boost/thread.hpp>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

// A message printer. The message from a queue.
template <typename T>
void print_message(T const &value)
{
    using namespace rediscpp::resp::deserialization;
    boost::apply_visitor(rediscpp::resp::detail::make_visitor(
           [] (bulk_string const &val)
           { std::cout << val.get() << std::endl; }
       ), value);
}

int main()
{
    try
    {
        auto const N = 100;
        auto const queue_name = "test_queue";

        bool volatile stopped = false;

        // The subscriber is runned in its own thread.
        // It's some artificial example, when publisher
        // and subscriber are working in one process.
        // It's only for demonstration library abilities.
        boost::thread subscriber{
            [&stopped, &queue_name]
            {
                // Its own stream for a subscriber
                auto stream = rediscpp::make_stream("localhost", "6379");
                auto response = rediscpp::execute(*stream, "subscribe", queue_name);
                // An almost endless loop for getting messages from the queues.
                while (!stopped)
                {
                    // Reading / waiting for a message.
                    rediscpp::value value{*stream};
                    // Message extraction.
                    boost::apply_visitor(rediscpp::resp::detail::make_visitor(
                            // We're wondered only an array in response.
                            // Otherwise, there is an error.
                            [] (rediscpp::resp::deserialization::array const &arr)
                            {
                                std::cout << "-------- Message --------" << std::endl;
                                for (auto const &i : arr.get())
                                    print_message(i);
                                std::cout << "-------------------------" << std::endl;
                            },
                            // Oops. An error in a response.
                            [] (rediscpp::resp::deserialization::error_message const &err)
                            { std::cerr << "Error: " << err.get() << std::endl; }
                        ), value.get());
                }
            }
        };

        // An artificial delay. It's not necessary in real code.
        std::this_thread::sleep_for(std::chrono::milliseconds{200});

        // Its own stream for a publisher.
        auto stream = rediscpp::make_stream("localhost", "6379");

        // The publishing N messages.
        for (int i = 0 ; i < N ; ++i)
        {
            auto response = rediscpp::execute(*stream,
                    "publish", queue_name, std::to_string(i));
            std::cout << "Delivered to " << response.as<std::int64_t>()
                      << " subscribers." << std::endl;
        }

        // An artificial delay. It's not necessary in real code.
        // It's due to the artificiality of the example,
        // where everything is in one process.
        std::this_thread::sleep_for(std::chrono::milliseconds{200});

        stopped = true;
        std::this_thread::sleep_for(std::chrono::milliseconds{200});
        // Why not?... Please, avoid it in real code.
        // It's justified only in examples.
        subscriber.interrupt();
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```  

# Conclusion  
Take a look at a code above one more time. I hope there's you could find something useful for your own projects with Redis. I'd thought to add one more level to wrap all Redis commands and refused this idea. A lot of useless work with a small outcome, because, in many cases we need to run only a handful of commands. Maybe it'll be a good idea in the future. Now you can use redis-cpp like lightweight library to execute Redis commands and get results  with minimal effort.  

**Enjoy your own projects with Redis!**  
