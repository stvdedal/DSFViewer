#ifndef OLHA_DSF_NAVMAP_THREAD_H_
#define OLHA_DSF_NAVMAP_THREAD_H_

#include "include_gl.h"
#include <navmap_thread.h>

class DsfNavMapThread : public NavMapThread
{
    IMarkerRender* createMarkerRender();
    IMapRender*    createMapRender();

    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;

    int _textId;

    GLfloat _left, _bottom, _width, _height;

public:
    DsfNavMapThread();
    ~DsfNavMapThread();

    void setRect(GLfloat left, GLfloat bottom, GLfloat width, GLfloat height);
    void render();
};

#endif
