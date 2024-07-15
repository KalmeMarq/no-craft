#include "Shader.hpp"

void Shader::Init(const char *name)
{
    std::string vertexSrc;
    std::string fragmentSrc;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.open(std::string("res/shaders/") + name + ".vsh.glsl");
    fShaderFile.open(std::string("res/shaders/") + name + ".fsh.glsl");
    
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexSrc = vShaderStream.str();
    fragmentSrc = fShaderStream.str();

    const char *vertexSource = vertexSrc.c_str();
    const char *fragmentSource = fragmentSrc.c_str();

    GLuint prog = glCreateProgram();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);

    int success;
    char infoLog[1024];

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        std::cerr << (GLchar*)infoLog << "\n";
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
        std::cerr << (GLchar*)infoLog << "\n";
    }

    glAttachShader(prog, vertex);
    glAttachShader(prog, fragment);

    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(prog, 1024, NULL, infoLog);
        std::cerr << (GLchar*)infoLog << "\n";
    }

    glDetachShader(prog, vertex);
    glDetachShader(prog, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    m_id = prog;
}

Shader::~Shader()
{
    if (this->m_id != -1) {
        glDeleteProgram(m_id);
    }
}

void Shader::Use()
{
    if (this->m_id != -1) {
        glUseProgram(m_id);
    }
}

void Shader::SetUniformBool(const std::string &name, bool value) {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::SetUniformInt(const std::string &name, int value) {
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::SetUniformFloat(const std::string &name, float value) {
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::SetUniformFloat4(const std::string &name, float value0, float value1, float value2, float value3) {
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), value0, value1, value2, value3);
}

void Shader::SetUniformMat4(const std::string &name, glm::mat4 &value) {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, false, glm::value_ptr(value));
}
