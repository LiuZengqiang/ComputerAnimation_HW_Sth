cmake_minimum_required(VERSION 3.17)
project(ComputerAnimation_HW_Sth)

set(CMAKE_CXX_STANDARD 14)
include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_executable(ComputerAnimation_HW_Sth main.cpp glad.c)
target_link_libraries(ComputerAnimation_HW_Sth PRIVATE ${PROJECT_SOURCE_DIR}/lib/libglfw3dll.a Shader Joint)