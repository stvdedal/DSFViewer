#include "simple_marker.h"

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

void SimpleMarker::setScale(double scale_x, double scale_y)
{

}

void SimpleMarker::setRotation(double angle)
{

}

void SimpleMarker::render()
{

}
