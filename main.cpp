// 在include glfw之前必须include glad
#include "src/Shader.h"
#include "src/Joint.h"
#include "glm/glm.hpp"
#include "src/globalFunction.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

vec2 target(0, 300);
//int index = 0;
METHODS method = METHODS::CCD;
//
//
//vector<vec2> targets = {
//        vec2(100, -100),
//        vec2(-100, -100),
//        vec2(-100, 100),
//        vec2(100, 100)
//};
float *data;
int data_cnt;
Joint *joint;

// frame buffer刷新回调函数
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void cursorPositionCallback(GLFWwindow *window, double x, double y);

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

// 输入回调函数
void processInput(GLFWwindow *window);

void updateVertices(Joint *joint);

int main(int argc, char **argv) {

    joint = new Joint(SCR_WIDTH, SCR_HEIGHT);

    data = new float[joint->getVerticesSize() * 3];
    data_cnt = joint->getVerticesSize();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    // 生成窗口window，GLFWwindow* window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CCD", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 设置上下文为新生成的window
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetKeyCallback(window, keyCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    Shader shader("../src/vertex_shader.vs", "../src/fragment_shader.fs");


    unsigned int VBO, VAO;

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(*data) * data_cnt * 3, data, GL_DYNAMIC_DRAW);


    glGenVertexArrays(1, &VAO);


    glBindVertexArray(VAO);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    // 启用顶点着色器(vertex shader)中的location=0的顶点属性
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {

        updateVertices(joint);

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清空屏幕缓冲
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        // 绑定vao
        glBindVertexArray(VAO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(*data) * data_cnt * 3, data, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        glLineWidth(3);
        glColorP4ui(2, 2);

        shader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
        glDrawArrays(GL_LINE_STRIP, 0, data_cnt);
        shader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));

        glPointSize(8);
        glDrawArrays(GL_POINTS, 0, data_cnt);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    joint->setScreenSize(width, height);
}

void cursorPositionCallback(GLFWwindow *window, double x, double y) {
    target = vec2(x - SCR_WIDTH / 2, -1 * (y - SCR_HEIGHT / 2));
    joint->setTarget(target);
    joint->updateJoints(method);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
//                target = targets[(index++) % targets.size()];
//                joint->setTarget(target);
//                joint->updateJoints(method);
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
//                printf("you push right button.\n");
            default:
                break;
        }
    }
}

// todo::update vertices
void updateVertices(Joint *joint) {
    delete[] data;
    vector<vec3> vertices_ = joint->getVertices();
    data_cnt = vertices_.size();
    data = new float[vertices_.size() * 3];

    for (int i = 0; i < vertices_.size(); i++) {
        data[i * 3 + 0] = vertices_[i].x;
        data[i * 3 + 1] = vertices_[i].y;
        data[i * 3 + 2] = vertices_[i].z;
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_SPACE) {
        switch (method) {
            case METHODS::CCD:
                method = METHODS::CJD;
                cout << "Switch to CJD():" << endl;
                break;
            case METHODS::CJD:
                method = METHODS::CCD;
                cout << "Switch to CCD():" << endl;
                break;
            default:
                method = METHODS::CCD;
                cout << "Switch to CCD():" << endl;
                break;
        }
    }
}

