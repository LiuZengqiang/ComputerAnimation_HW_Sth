cmake_minimum_required(VERSION 3.17)
project(OpenGLHelloworld)

set(CMAKE_CXX_STANDARD 14)
include_directories("E:/GLFW/include")
include_directories("E:/glm")
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_executable(OpenGLHelloworld main.cpp glad.c)

target_link_libraries(OpenGLHelloworld PRIVATE E:/GLFW/lib/libglfw3dll.a Shader Joint)
#target_link_libraries(OpenGLHelloworld PRIVATE E:/GLFW/lib/libglfw3dll.a MyOpengl)