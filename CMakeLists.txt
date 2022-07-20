cmake_minimum_required(VERSION 3.5)

project(Sinusoida LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -g -lstdc++fs")
set(CMAKE_CPP_COMLILER g++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2)
include_directories(${SDL2_INCLUDE_DIRS})

file(GLOB SRC "source/*.cpp")
file(GLOB HDR "source/headers/*.hpp")

add_executable(${PROJECT_NAME} ${SRC} ${HDR})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
