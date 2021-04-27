// 在include glfw之前必须include glad
#include "src/Shader.h"
#include "src/Joint.h"
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include "src/glClass.h"

// settings
// 窗口宽高
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

double cursor_pos_x = 0;
double cursor_pos_y = 0;

vec2 target(0, 300);

vector<float> joint_vec = {100, 100, 50, 50};
vector<vec2> targets = {
        vec2(0, 200),
        vec2(100, 100),
        vec2(100, -100),
        vec2(-100, -100),
        vec2(-100, 100)
};
//vector<vec2> targets = {
////        vec2(0, 100),
//        vec2(100, 100),
//        vec2(100, 150),
//        vec2(100, 200),
//        vec2(100, 250),
//        vec2(100, 300),
//        vec2(100, 250),
//        vec2(100, 200),
//        vec2(100, 150)
//};
//vector<vec2> targets = {
////        vec2(0, 100),
//        vec2(50, 100),
//        vec2(100, -100),
//        vec2(150, -100),
//        vec2(200, 100),
//        vec2(0, 100),
//        vec2(100, 0)
//};
int index = 0;

// 点位置数据
float *data;
int data_cnt = 4;
float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f, -0.5f, 0.0f, // right
        0.0f, 0.5f, 0.0f,  // top
        0.0f, 0.0f, 0.0f
};
Joint joint(joint_vec, SCR_WIDTH, SCR_HEIGHT);

// frame buffer刷新回调函数
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void cursorPositionCallback(GLFWwindow *window, double x, double y);

// 输入回调函数
void processInput(GLFWwindow *window);

void updateVertices(Joint &joint);


// vertex shader程序
// 点着色程序
// 程序版本 version 330 core
// 第一个uniform 变量 aPos
// main()函数
// 设置gl_Position = aPos
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

// fragment shader程序
// 段着色程序
// 程序版本 version 330 core
// 输出(out)变量 FragColor
// main()函数
// 初始化FragColor
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

int main() {


    joint.debug();

    data = new float[joint_vec.size() * 3];

    vector<float> temp = {-0.5f, -0.5f, 0.0f,
                          0.5f, -0.5f, 0.0f,
                          0.0f, 0.5f, 0.0f};

    for (int i = 0; i < joint_vec.size() * 3; i++) {
        data[i] = temp[i];
    }
    data_cnt = joint_vec.size();
    // glfw: initialize and configure
    // ------------------------------
    // 初始化glfw，设置glfw版本3.3, 大版本号3，小版本号也是3，设置使用opengl_core模式
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // 设置上下文为新生成的window
    glfwMakeContextCurrent(window);
    // 设置window的framebuffer回调函数
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    // glad用于加载opengl的函数指针
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
}

void cursorPositionCallback(GLFWwindow *window, double x, double y) {
    cursor_pos_x = x;
    cursor_pos_y = y;
    cout << x << " " << y << endl;
    target = vec2(x - SCR_WIDTH / 2, -1 * (y - SCR_HEIGHT / 2));
    cout << "target:(" << target.x << "," << target.y << ")" << endl;
    joint.setTarget(target);
//    joint.updateJointsCCD();
//    joint.updateJointsRCCD();
//    joint.updateJointsCC();
    joint.updateJointsMCD();
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT:
                printf("you push left button.\n");
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                printf("you push right button.\n");
            default:
                break;
        }
    }
}

// todo::update vertices
void updateVertices(Joint &joint) {
    delete[] data;
    vector<vec3> vertices_ = joint.getVertices();
    data_cnt = vertices_.size();
    data = new float[vertices_.size() * 3];

    for (int i = 0; i < vertices_.size(); i++) {
        data[i * 3 + 0] = vertices_[i].x;
        data[i * 3 + 1] = vertices_[i].y;
        data[i * 3 + 2] = vertices_[i].z;
    }
}
