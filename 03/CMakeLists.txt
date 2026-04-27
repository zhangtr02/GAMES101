cmake_minimum_required(VERSION 3.10)
project(Shading)

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(Shading src/main.cpp src/rasterizer.cpp src/Triangle.cpp src/Texture.cpp)
target_link_libraries(Shading PRIVATE Eigen3::Eigen ${OpenCV_LIBS})