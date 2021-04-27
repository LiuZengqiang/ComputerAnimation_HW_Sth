message("----- Begin Shader Scene -----")

add_library(Shader Shader.h Shader.cpp)
add_library(Joint Joint.h Joint.cpp)

add_library(MyOpengl glClass.h glClass.cpp)
target_link_libraries(MyOpengl PUBLIC E:/GLFW/lib/libglfw3dll.a)
message("----- End Shader Scene -----")