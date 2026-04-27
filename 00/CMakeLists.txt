cmake_minimum_required(VERSION 3.10)
project(Transformation)

find_package(Eigen3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(Transformation src/main.cpp)
target_link_libraries(Transformation PRIVATE Eigen3::Eigen)