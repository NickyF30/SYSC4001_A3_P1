cmake_minimum_required(VERSION 4.0)
project(Assignment_3)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Assignment_3
        build.sh
        interrupts_101304731_101296691.hpp
        interrupts_101304731_101296691_EP.cpp
        interrupts_101304731_101296691_EP_RR.cpp
        interrupts_101304731_101296691_RR.cpp)
