cmake_minimum_required(VERSION 3.17)
project(ComputerAnimation_HW_Sth)

set(CMAKE_CXX_STANDARD 14)
include_directories("E:/GLFW/include")
include_directories("E:/glm")
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_executable(ComputerAnimation_HW_Sth main.cpp glad.c)

target_link_libraries(ComputerAnimation_HW_Sth PRIVATE E:/GLFW/lib/libglfw3dll.a Shader Joint)
#target_link_libraries(OpenGLHelloworld PRIVATE E:/GLFW/lib/libglfw3dll.a MyOpengl)