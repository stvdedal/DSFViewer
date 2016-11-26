#include "simple_marker.h"
#include <shader_maker.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const GLchar* SimpleMarker::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(position, 1.0);
}
)";

const GLchar* SimpleMarker::fragmentShaderSource = R"(
#version 330 core
out vec4 color;
void main()
{
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)";

const GLfloat SimpleMarker::vertexData[] =
{
    -0.5F, -0.5F, 0.0F,
    +0.5F, -0.5F, 0.0F,
    +0.0F, +1.0F, 0.0F,
};

const GLuint SimpleMarker::vertexElements[] = { 0, 1, 2 };

SimpleMarker::SimpleMarker()
{
    _scale_x = 1.0f;
    _scale_y = 1.0f;
    _rotation = 0.0f;

    _program = makeProgram(vertexShaderSource, fragmentShaderSource);
    renderable.loadData(vertexData, sizeof(vertexData));
    renderable.loadElements(vertexElements, sizeof(vertexElements));
    renderable.setAttrib(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), 0);
    renderable.setParam(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

SimpleMarker::~SimpleMarker()
{
    glDeleteProgram(_program);
}

void SimpleMarker::setScale(double scale_x, double scale_y)
{
    _scale_x = GLfloat(scale_x);
    _scale_y = GLfloat(scale_y);

    glm::mat4 transform;
    transform = glm::rotate(transform, glm::radians(-_rotation), glm::vec3(0.0, 0.0, 1.0));
    transform = glm::scale(transform, glm::vec3(_scale_x, _scale_y, 1.0f));
    _transform = transform;
}

void SimpleMarker::setRotation(double angle)
{
    _rotation = GLfloat(angle);

    glm::mat4 transform;
    transform = glm::rotate(transform, glm::radians(-_rotation), glm::vec3(0.0, 0.0, 1.0));
    transform = glm::scale(transform, glm::vec3(_scale_x, _scale_y, 1.0f));
    _transform = transform;
}

void SimpleMarker::render()
{
    glUseProgram(_program);
    GLint transformLoc = glGetUniformLocation(_program, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(_transform));

    renderable();
}
