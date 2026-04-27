cmake_minimum_required(VERSION 3.10)
project(Z-buffering)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(Z-buffering src/main.cpp src/rasterizer.cpp src/Triangle.cpp)
target_link_libraries(Z-buffering PRIVATE Eigen3::Eigen ${OpenCV_LIBS})
