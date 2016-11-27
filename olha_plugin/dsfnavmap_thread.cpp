#include "dsfnavmap_thread.h"
#include <dsf_map.h>
#include <simple_marker.h>

#include <XPLMGraphics.h>

#if 0
const char* DsfNavMapThread::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;
out vec2 texCoord;
void main()
{
    gl_Position = vec4(position, 1.0);
    texCoord = tex;
}
)";

const char* DsfNavMapThread::fragmentShaderSource = R"(
#version 330 core
in vec2 texCoord;
out vec4 color;
uniform sampler2D texSampler;
void main()
{
    color = texture(texSampler, texCoord);
}
)";
#endif

IMarkerRender* DsfNavMapThread::createMarkerRender()
{
    return new SimpleMarker;
}

IMapRender* DsfNavMapThread::createMapRender()
{
    return new DsfMap;
}

DsfNavMapThread::DsfNavMapThread()
{
    XPLMGenerateTextureNumbers(&_textId, 1);
    XPLMBindTexture2d(_textId, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

DsfNavMapThread::~DsfNavMapThread()
{
    GLuint id = _textId;
    glDeleteTextures(1, &id);
}

void DsfNavMapThread::setRect(GLfloat left, GLfloat bottom, GLfloat width, GLfloat height)
{
    _left = left;
    _bottom = bottom;
    _width = width;
    _height = height;
}

void DsfNavMapThread::render()
{
    XPLMBindTexture2d(_textId, 0);

    _texture.guard.lock();
    if (!_texture.dirty)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _texture.width, _texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, _texture.buf.data());
    _texture.guard.unlock();

    XPLMSetGraphicsState(
        0,        // No fog, equivalent to glDisable(GL_FOG);
        1,        // One texture, equivalent to glEnable(GL_TEXTURE_2D);
        0,        // No lighting, equivalent to glDisable(GL_LIGHT0);
        0,        // No alpha testing, e.g glDisable(GL_ALPHA_TEST);
        0,        // Use alpha blending, e.g. glEnable(GL_BLEND);
        0,        // No depth read, e.g. glDisable(GL_DEPTH_TEST);
        0);       // No depth write, e.g. glDepthMask(GL_FALSE);

    glColor3f(1, 1, 1);        // Set color to white.
    GLfloat x1 = _left;
    GLfloat y1 = _bottom;
    GLfloat x2 = x1 + _width;
    GLfloat y2 = y1 + _height;
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0, 0);        glVertex2f(x1, y1);
    glTexCoord2f(0, 1);        glVertex2f(x1, y2);
    glTexCoord2f(1, 1);        glVertex2f(x2, y2);
    glTexCoord2f(1, 0);        glVertex2f(x2, y1);
    glEnd();
}
