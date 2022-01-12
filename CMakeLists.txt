cmake_minimum_required(VERSION 3.21)
project(concepts LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME})

add_subdirectory(src)

include(cmake/Conan.cmake)
run_conan()

find_package(fmt 8.0.1 REQUIRED)
find_package(Boost 1.78.0 REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    fmt::fmt
    Boost::Boost
    )