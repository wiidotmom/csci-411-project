cmake_minimum_required(VERSION 3.16)
project(project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SDL3_BUILD_EXAMPLES OFF)
option(SDL3_BUILD_TESTS OFF)

include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
)
FetchContent_MakeAvailable(SDL3)

add_executable(project src/main.cpp)

target_link_libraries(project
    SDL3::SDL3
)