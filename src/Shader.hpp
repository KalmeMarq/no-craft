#pragma once

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    ~Shader();

    void Init(const char *name);

    void Use();
    void SetUniformBool(const std::string &name, bool value);
    void SetUniformInt(const std::string &name, int value);
    void SetUniformFloat(const std::string &name, float value);
    void SetUniformFloat4(const std::string &name, float value0, float value1, float value2, float value3);
    void SetUniformMat4(const std::string &name, glm::mat4 &value);
private:
    GLuint m_id = -1;
};
