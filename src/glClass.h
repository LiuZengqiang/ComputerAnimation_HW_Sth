//
// Created by Sth on 2021/4/21.
//

#ifndef OPENGLHELLOWORLD_GLCLASS_H
#define OPENGLHELLOWORLD_GLCLASS_H

#include "Shader.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

using namespace std;

class MyOpengl {
public:
    MyOpengl();

private:
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n\0";


    void cursorPositionCallback(GLFWwindow *window, double x, double y) {
        printf("cursor position is (%f,%f) IN MY OPENGL CLASS\n", x, y);
    }

    static void processInput(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    void debug(int *value, string suf = "") {
        // 32位
        int num = *value;
        for (int i = 31; i > -0; i--) {
            if (num >> i & 1) {
                cout << "1";
            } else {
                cout << "0";
            }
        }
        cout << suf;
    }


};

#endif //OPENGLHELLOWORLD_GLCLASS_H
