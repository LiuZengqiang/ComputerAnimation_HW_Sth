//
// Created by Sth on 2021/4/21.
//
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char *vertex_shader_code_path, const char *fragment_shader_code_path) {

    // 1. retrieve the shader source code from file
    string vertex_shader_code;
    string fragment_shader_code;
    ifstream vertex_shader_file;
    ifstream fragment_shader_file;
    try {
        vertex_shader_file.open(vertex_shader_code_path);
        fragment_shader_file.open(fragment_shader_code_path);

        stringstream vertex_shader_ss;
        stringstream fragment_shader_ss;
        vertex_shader_ss << vertex_shader_file.rdbuf();
        fragment_shader_ss << fragment_shader_file.rdbuf();
        vertex_shader_file.close();
        fragment_shader_file.close();

        vertex_shader_code = vertex_shader_ss.str();
        fragment_shader_code = fragment_shader_ss.str();

    } catch (ifstream::failure &e) {
        cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    // 2. compile vertex/fragment shader
    const char *vertex_shader_code_c = vertex_shader_code.c_str();
    const char *fragment_shader_code_c = fragment_shader_code.c_str();
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code_c, nullptr);
    glCompileShader(vertex_shader);
    checkCompileErrors(vertex_shader, CHECK_TYPE::SHADER, "vertex_shader");

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code_c, nullptr);
    glCompileShader(fragment_shader);
    checkCompileErrors(fragment_shader, CHECK_TYPE::SHADER, "fragment_shader");

    // 3. compile shader program
    Id_ = glCreateProgram();
    glAttachShader(Id_, vertex_shader);
    glAttachShader(Id_, fragment_shader);
    glLinkProgram(Id_);
    checkCompileErrors(Id_, CHECK_TYPE::PROGRAM, "shader_program");

    // 4. delete the shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::checkCompileErrors(GLuint shader, Shader::CHECK_TYPE check_type, string name) {
    int success;
    char infoLog[1024];
    switch (check_type) {
        case CHECK_TYPE::SHADER:
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                cout << "ERROR::SHADER_COMPILATION_ERROR of type:" << name << endl;
                cout << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
            break;
        case CHECK_TYPE::PROGRAM:
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type:" << name << endl;
                cout << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
            break;
    }
}

GLuint Shader::getShaderId() {
    return Id_;
}

void Shader::use() {
    glUseProgram(Id_);
}

Shader::~Shader() {
    glDeleteProgram(Id_);
}

void Shader::setVec3(const string &name, const glm::vec3& value) const {

    glUniform3fv(glGetUniformLocation(Id_, name.c_str()), 1, &value[0]);
}





