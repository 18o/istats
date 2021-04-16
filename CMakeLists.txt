cmake_minimum_required(VERSION 3.17)
project(smc)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(smc
        main.cpp
        power.cpp
        smc.cpp
        test.cpp
        smc.h
        power.h)
target_link_libraries(${PROJECT_NAME} "-framework IOKit")
target_link_libraries(${PROJECT_NAME} "-framework CoreFoundation")


