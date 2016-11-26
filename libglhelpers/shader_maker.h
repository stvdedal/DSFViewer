#ifndef SHADER_MAKER_H_
#define SHADER_MAKER_H_

#include <GL/glew.h>

GLuint makeShader(GLenum shaderType, const GLchar* source);
GLuint makeProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);

#endif
