#include "shader_maker.h"
#include <iostream>

GLuint makeShader(GLenum shaderType, const GLchar* source)
{
    GLint success;
    GLchar infoLog[512];
    GLuint shader;

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "ERROR: SHADER COMPILATION FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint makeProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
{
    GLuint vertShader = makeShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragShader = makeShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    GLint success;
    GLchar infoLog[512];

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        std::cerr << "ERROR: SHADER LINK FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}
