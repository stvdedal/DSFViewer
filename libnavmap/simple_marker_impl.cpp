#include "simple_marker_impl.h"
#include <shader_maker.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const GLchar* SimpleMarkerImpl::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(position, 1.0);
}
)";

const GLchar* SimpleMarkerImpl::fragmentShaderSource = R"(
#version 330 core
out vec4 color;
void main()
{
    color = vec4(0.9f, 0.0f, 0.0f, 0.6f);
}
)";

const GLfloat SimpleMarkerImpl::vertexData[] =
{
    -0.5F, -0.5F, 0.0F,
    +0.5F, -0.5F, 0.0F,
    +0.0F, +1.0F, 0.0F,
};

const GLuint SimpleMarkerImpl::vertexElements[] = { 0, 1, 2 };

SimpleMarkerImpl::SimpleMarkerImpl()
{
    _x = 0.0f;
    _y = 0.0f;
    _scale_x = 1.0f;
    _scale_y = 1.0f;
    _rotation = 0.0f;

    _program = makeProgram(vertexShaderSource, fragmentShaderSource);
    renderable.loadData(vertexData, sizeof(vertexData));
    renderable.loadElements(vertexElements, sizeof(vertexElements));
    renderable.setAttrib(0, 3, GL_FLOAT, 3 * sizeof(GLfloat), 0);
    renderable.setParam(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

SimpleMarkerImpl::~SimpleMarkerImpl()
{
    glDeleteProgram(_program);
}

void SimpleMarkerImpl::setTransform()
{
    glm::mat4 transform;
    transform = glm::translate(transform, glm::vec3(_x, _y, 0.0f));
    transform = glm::rotate(transform, glm::radians(-_rotation), glm::vec3(0.0, 0.0, 1.0));
    transform = glm::scale(transform, glm::vec3(_scale_x, _scale_y, 1.0f));
    _transform = transform;
}

void SimpleMarkerImpl::setTranslate(double x, double y)
{
    std::cerr << "Translate: x=" << x << " y=" << y << std::endl;

    _x = GLfloat(x);
    _y = GLfloat(y);
    setTransform();
}

void SimpleMarkerImpl::setScale(double scale_x, double scale_y)
{
    _scale_x = GLfloat(scale_x);
    _scale_y = GLfloat(scale_y);
    setTransform();
}

void SimpleMarkerImpl::setRotation(double angle)
{
    _rotation = GLfloat(angle);
    setTransform();
}

void SimpleMarkerImpl::render()
{
    glUseProgram(_program);
    GLint transformLoc = glGetUniformLocation(_program, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(_transform));

    renderable();
}
