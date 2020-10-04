cmake_minimum_required(VERSION 3.12.0)

#-----------------------Options--------------------------------------
option (REDISCPP_HEADER_ONLY "[REDISCPP] Header only" OFF)
option (REDISCPP_EASY_ADDRESS_RESOLVE "[REDISCPP] Use easy address resolving" OFF)
#--------------------------------------------------------------------

project(redis-cpp)
set(PROJECT ${PROJECT_NAME})
string(TOLOWER "${PROJECT}" PROJECT_LC)

if (REDISCPP_HEADER_ONLY)
    set (REDISCPP_DEFINES "${REDISCPP_DEFINES} -DREDISCPP_HEADER_ONLY")
endif()

if (REDISCPP_EASY_ADDRESS_RESOLVE)
    set(REDISCPP_DEFINES, "${REDISCPP_DEFINES} -DREDISCPP_EASY_ADDRESS_RESOLVE")
endif()

set (STD_CXX "c++11")

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/MyCMakeScripts)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)


set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall ${REDISCPP_DEFINES} -std=${STD_CXX}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -W -Wall ${REDISCPP_DEFINES} -std=${STD_CXX} -DNDEBUG")
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

#---------------------------------------------------------

find_package(Boost 1.67.0 REQUIRED COMPONENTS thread system iostreams)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set (LIBRARIES
    ${LIBRARIES}
    ${Boost_LIBRARIES}
)

if (NOT REDISCPP_HEADER_ONLY)
    set(SOURCES
        ${SOURCES}
        src/redis-cpp/stream.cpp
    )
endif()

include_directories (include)
if (NOT REDISCPP_HEADER_ONLY)
    add_library (${PROJECT_LC} STATIC ${SOURCES})
    target_link_libraries (${PROJECT_LC} ${LIBRARIES})
endif()

set (LIBRARIES
        ${PROJECT_LC}
        ${LIBRARIES}
    )

install(DIRECTORY include/${PROJECT_LC} DESTINATION include)

if (NOT REDISCPP_HEADER_ONLY)
    install (TARGETS ${PROJECT_LC} EXPORT ${PROJECT_LC}
                 ARCHIVE DESTINATION lib
    )
endif()
