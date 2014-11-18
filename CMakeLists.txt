cmake_minimum_required(VERSION 2.8.4)
project(xpath)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -std=c++11")

set(SOURCE_FILES xpath.cpp lib/pugixml/src/pugixml.cpp)
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

# Boost
find_package(Boost REQUIRED filesystem system program_options)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(xpath ${SOURCE_FILES})
target_link_libraries(xpath ${Boost_LIBRARIES})