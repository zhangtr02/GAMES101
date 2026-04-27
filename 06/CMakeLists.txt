cmake_minimum_required(VERSION 3.10)
project(BVH)

set(CMAKE_CXX_STANDARD 17)

add_executable(BVH src/main.cpp src/Vector.cpp src/Scene.cpp src/BVH.cpp src/Renderer.cpp)