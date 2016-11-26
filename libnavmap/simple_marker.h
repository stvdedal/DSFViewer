#ifndef NAVMAP_SIMPLE_MARKER_H_
#define NAVMAP_SIMPLE_MARKER_H_

#include <GL/glew.h>
#include <imarker_render.h>

class SimpleMarker : public IMarkerRender
{
    static const GLchar* vertexShaderSource;
    static const GLchar* fragmentShaderSource;
public:
    virtual void setScale(double scale_x, double scale_y);
    virtual void setRotation(double angle);
    virtual void render();
};

#endif
