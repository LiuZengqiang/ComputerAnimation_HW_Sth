//
// Created by Sth on 2021/4/21.
//

#include "glClass.h"
#include "string"
#include <iostream>
#include <stdio.h>
#include <functional>
#include <cstring>

MyOpengl::MyOpengl() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    // 设置上下文为新生成的window
    glfwMakeContextCurrent(window);
    // 设置window的framebuffer回调函数
    // little tick
    union {
        void (*dest)(GLFWwindow *, double, double);

        void (MyOpengl::*src)(GLFWwindow *, double, double);
    } u;
    u.src = &MyOpengl::cursorPositionCallback;
    glfwSetCursorPosCallback(window, u.dest);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 函数参数：要编译的shader对象、源码的字符串数目、shader源码、指定源码字符串的长度(null表示以字符串的null为结尾)
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // 编译shader
    glCompileShader(vertexShader);
    // check for shader compile errors
    // 检查是否编译成功
    int success;
    char infoLog[512];
    // get shader information value?
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    // 构建编译段着色器
    // 创建一个段着色器 fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 函数参数：要编译的shader对象、源码的字符串数目、shader源码、指定源码字符串的长度(null表示以字符串的null为结尾)
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // 编译shader
    glCompileShader(fragmentShader);

    // check for shader compile errors
    // 检查是否编译成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    // 链接shader
    // 创建着色器程序shader program
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // 当着色器程序编译成功后，可以将之前的着色器删除
    // 着色器"程序"编译成功后，可以将"着色器"删除！
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 点位置数据
    float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left
            0.5f, -0.5f, 0.0f, // right
            0.0f, 0.5f, 0.0f,  // top
            0.0f, 0.0f, 0.0f
    };

    unsigned int VBO, VAO;
    // 创建一个vbo
    glGenBuffers(1, &VBO);
    // 设定VBO(变量)缓冲类型为GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 将cup中的内存复制到缓冲内存中，并指定数据格式
    // 函数参数：缓冲类型、数据大小(以字节为单位)、cpu数据地址、数据处理类型(GL_STATIC_DRAW:数据基本不改变；GL_DYNAMIC_DRAW:数据经常改变；GL_STREAM_DRAW:数据每次绘制之后都会改变)
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 创建一个vao，顶点数组对象
    glGenVertexArrays(1, &VAO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    // 绑定顶点数组对象vao
    glBindVertexArray(VAO);
    // 设置顶点的属性 glVertexAttribPointer()
    // 函数参数：需要配置的顶点属性(就是顶点着色器中loction=0)，顶点属性的大小(字节)，数据的类型(float)，数据是否被标准化，
    // 数据步长，数据在缓冲中的起始位置的偏移
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    // 启用顶点着色器(vertex shader)中的location=0的顶点属性
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's
    // bound vertex buffer object so afterwards we can safely unbind
    // 到这里我们已经创建绑定了VAO,并且创建绑定VBO并将顶点数据复制到VBO管理的显存中，同时使用glVertexAttribPointer()告诉opengl
    // 的顶点着色器如何使用VBO中的数据(这些数据给顶点着色器的哪个输入属性，读取这些数据的格式如何)
    // 因此可以解绑GL_ARRAY_BUFFER(或者将其与另一个VBO绑定，设定其他新的顶点属性)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // 同时也可以解绑VAO，只要在绘制时绑定特定的VAO就可以成功绘制
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        // 使用指定颜色填充屏幕
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清空屏幕缓冲
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        // 使用之前编译好的着色程序
        glUseProgram(shaderProgram);
//        glUseProgram(shader.getShaderId());
//        shader.use();
        // 绑定vao
        glBindVertexArray(
                VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // 设定绘制图元primitive
        // 这里设置的图元为:GL_TRIANGLES，数组缓存(VBO)的起始索引，点的个数
        // 例如，我们的原始数据vertices中有四组数据(四个顶点数据)，同时之后我们将这四个顶点数据都复制到VBO中
        // 在glVertexAttribPointer()中又设置了这个数据与顶点着色器中location=0的输入数据绑定，并且读取格式为每三个float为一组
        // 那么顶点着色器就能收到四个输入数据包(一个输入数据表示一个坐标点)
        // 而假如我们使用glDrawArrays(GL_TRIANGLES,0,3)时，会使用第1,2,3(下标为0起始)个点绘制一个三角形
        // 而假如我们使用glDrawArrays(GL_TRIANGLES,1,3)时，会使用第2,3,4(下标为1起始)个点绘制一个三角形
//        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 1, 3);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
//    glDeleteProgram(shader.getShaderId());
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();


}
