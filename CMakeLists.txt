cmake_minimum_required(VERSION 4.0)
project(Assignment_3)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Assignment_3
        build.sh
        interrupts_student1_student2.hpp
        interrupts_student1_student2_EP.cpp
        interrupts_student1_student2_EP_RR.cpp
        interrupts_student1_student2_RR.cpp)
