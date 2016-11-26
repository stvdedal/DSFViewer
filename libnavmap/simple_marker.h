#ifndef NAVMAP_SIMPLE_MARKER_H_
#define NAVMAP_SIMPLE_MARKER_H_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <imarker_render.h>
#include <drawable_object.h>

class SimpleMarker : public IMarkerRender
{
    static const GLchar* vertexShaderSource;
    static const GLchar* fragmentShaderSource;

    GLuint _program;

    static const GLfloat vertexData[];
    static const GLuint vertexElements[];
    DrawElements renderable;

    GLfloat _scale_x;
    GLfloat _scale_y;
    GLfloat _rotation;

    glm::mat4 _transform;

public:
    SimpleMarker();
    ~SimpleMarker();

    virtual void setScale(double scale_x, double scale_y);
    virtual void setRotation(double angle);
    virtual void render();
};

#endif
