#include "dsf_map_thread.h"
#include "dsf_map.h"
#include "gl_context.h"

#include <iostream>

void MapTextureProvider::work()
{
    GlContext context;
    DsfMap dsfMap;

    GLuint FBO;
    glGenFramebuffers(1, &FBO);

    std::unique_lock<std::mutex> lck(_mtx);

    while (_working)
    {
        if (_result)
        {
            _result->guard.lock();
            bool dirty = _result->dirty;
            _result->guard.unlock();

            if (dirty)
            {
                glViewport(0, 0, _width, _height);

                glBindFramebuffer(GL_FRAMEBUFFER, FBO);

                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
                {
                    GLfloat lon = _lon;
                    GLfloat lat = _lat;
                    GLfloat scale_x = _scale_x;
                    GLfloat scale_y = _scale_y;

                    lck.unlock();
                    dsfMap.prepare(lon, lat, scale_x, scale_y);
                    dsfMap.render();
                    lck.lock();

                    _result->guard.lock();
                    _result->dirty = false;
                    _result->width = _width;
                    _result->height = _height;
                    _result->buf.clear();
                    _result->buf.resize(_width * _height * 3, 0xFF);

                    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, _result->buf.data());

                    _result->guard.unlock();
                }
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1, &texture);
            }
        }

        _cv.wait(lck);
    }

    glDeleteFramebuffers(1, &FBO);
}

MapTextureProvider::MapTextureProvider()
{
    _working = false;
}

MapTextureProvider::~MapTextureProvider()
{
}

void MapTextureProvider::start(GLuint width, GLuint height)
{
    if (_working)
        return;

    _width = width;
    _height = height;

    _working = true;
    _th = std::thread(&MapTextureProvider::work, this);
}

void MapTextureProvider::stop()
{
    if (!_working)
        return;
    {
        std::unique_lock<std::mutex> lck(_mtx);

        if (_working)
        {
            _working = false;
            _cv.notify_all();
        }
    }
    _th.join();
}

void MapTextureProvider::get(
    GLfloat lon, GLfloat lat,
    GLfloat scale_x, GLfloat scale_y,
    FutureTexture* result)
{
    std::unique_lock<std::mutex> lck(_mtx);

    result->guard.lock();
    result->dirty = true;
    result->guard.unlock();

    _lon = lon;
    _lat = lat;
    _scale_x = scale_x;
    _scale_y = scale_y;
    _result = result;

    _cv.notify_all();
}
