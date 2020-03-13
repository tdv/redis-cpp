cmake_minimum_required(VERSION 2.8)

include(ExternalProject)

project(my_app)
set(PROJECT ${PROJECT_NAME})
string(TOLOWER "${PROJECT}" PROJECT_LC)

set (STD_CXX "c++17")

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/MyCMakeScripts)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -std=${STD_CXX}")
set (CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -DNDEBUG")
set (CMAKE_POSITION_INDEPENDENT_CODE ON)

#---------------------------------------------------------

#---------------------Third party common -----------------

set (THITD_PARTY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/third_party)
set (THIRD_PARTY_PROJECTS "")
set (LIBRARIES "")

set (BOOST_URL "https://sourceforge.net/projects/boost/files/boost/1.72.0/boost_1_72_0.tar.gz")


#------------------------ Boost --------------------------
set (BOOST_LIBRARIES
    "thread"
    "system"
)

set (BOOST_INSTALL_DIR ${THITD_PARTY_OUTPUT_PATH}/boost)
set (BOOST_INCLUDE_DIR ${BOOST_INSTALL_DIR}/include)
set (BOOST_LIBRARIES_DIR ${BOOST_INSTALL_DIR}/lib)
include_directories (SYSTEM ${BOOST_INCLUDE_DIR})
link_directories(${BOOST_LIBRARIES_DIR})

set (BOOST_PROJECT "boost-project")
list (APPEND THIRD_PARTY_PROJECTS ${BOOST_PROJECT})

foreach (lib ${BOOST_LIBRARIES})
    if (DEFINED BOOST_LIBRARIES_LIST)
        set (BOOST_LIBRARIES_LIST "${BOOST_LIBRARIES_LIST},")
    endif()
    set (BOOST_LIBRARIES_LIST "${BOOST_LIBRARIES_LIST}${lib}")
endforeach()

ExternalProject_Add(${BOOST_PROJECT}
        URL ${BOOST_URL}
        UPDATE_COMMAND ""
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ./bootstrap.sh --prefix=${BOOST_INSTALL_DIR} --with-libraries=${BOOST_LIBRARIES_LIST} --without-icu
        BUILD_COMMAND ./b2 install -j8 --disable-icu --ignore-site-config "cxxflags=-std=${STD_CXX} -fPIC" link=static threading=multi runtime-link=static
        INSTALL_COMMAND ""
        LOG_DOWNLOAD 1
        LOG_UPDATE 1
        LOG_CONFIGURE 1
        LOG_BUILD 1
        LOG_TEST 1
        LOG_INSTALL 1
    )

foreach (lib ${BOOST_LIBRARIES})
    list (APPEND LIBRARIES boost_${lib})
endforeach()
#---------------------------------------------------------


set(HEADERS
    ${HEADERS}
    ${COMMON_HEADERS}
)

set(SOURCES
    ${SOURCES}
    src/redis-cpp/client.cpp
    src/redis-cpp/request.cpp
)

set(LIBRARIES
    ${LIBRARIES}
    pthread
    rt
)

include_directories (include)
include_directories (${HEADERS})

add_executable(${PROJECT_LC} ${HEADERS} ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(${PROJECT_LC} ${LIBRARIES})
add_dependencies (${PROJECT_LC} ${THIRD_PARTY_PROJECTS})
