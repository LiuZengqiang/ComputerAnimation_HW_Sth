//
// Created by Sth on 2021/4/21.
//

#ifndef OPENGLHELLOWORLD_SHADER_H
#define OPENGLHELLOWORLD_SHADER_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <string>

using namespace std;

class Shader {
public:
    // construct function
    Shader(const char *vertex_shader_code_path, const char *fragment_shader_code_path);

    ~Shader();

    GLuint getShaderId();

    void use();

    void setVec3(const string &name, const glm::vec3 &value) const;

private:

    enum class CHECK_TYPE {
        SHADER,
        PROGRAM
    };

    void checkCompileErrors(unsigned int shader, CHECK_TYPE check_type, string name);

    GLuint Id_;
    string vertex_shader_;
    string fragment_shader_;
};

#endif //OPENGLHELLOWORLD_SHADER_H
