cmake_minimum_required(VERSION 3.13)
project(project_1)

set(CMAKE_CXX_STANDARD 14)

add_executable(Toolkit1 socketApp1.cpp)
if(WIN32)
    target_link_libraries(Toolkit1 wsock32 ws2_32)
endif()