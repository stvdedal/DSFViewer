#include "navmap_thread.h"
#include "gl_context.h"
#include <gl_check_error.h>
#include "navmap.h"

#include <iostream>

void NavMapThread::work()
{
    GlContext context;

    IMapRender* mapRender = createMapRender();
    IMarkerRender* markerRender = createMarkerRender();
    NavMap* navMap = new NavMap(mapRender, markerRender);
    
    GLuint FBO;
    glGenFramebuffers(1, &FBO);

    std::unique_lock<std::mutex> lck(_mtx);

    while (_working)
    {
        _texture.guard.lock();
        bool dirty = _texture.dirty;
        _texture.guard.unlock();

        if (dirty)
        {
            glViewport(0, 0, _width, _height);

            glBindFramebuffer(GL_FRAMEBUFFER, FBO);

            GLuint TEX;
            glGenTextures(1, &TEX);
            glBindTexture(GL_TEXTURE_2D, TEX);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX, 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
            {
                double planeMarker_Lon = _PlaneMarker_Lon;
                double planeMarker_Lat = _PlaneMarker_Lat;
                double planeMarker_Hdg = _PlaneMarker_Hdg;
                double planeMarker_Scale_X = _PlaneMarker_Scale_X;
                double planeMarker_Scale_Y = _PlaneMarker_Scale_Y;
                double map_Scale_X = _Map_Scale_X;
                double map_Scale_Y = _Map_Scale_Y;

                lck.unlock();
                glClearColor(1.0f, 1.0f, 1.0f, 0.3f);
                glClear(GL_COLOR_BUFFER_BIT);

                navMap->setPlane(planeMarker_Lon, planeMarker_Lat, planeMarker_Hdg);
                navMap->setScale(map_Scale_X, map_Scale_Y);
                navMap->setPlaneScale(planeMarker_Scale_X, planeMarker_Scale_Y);
                navMap->render();
                lck.lock();

                _texture.guard.lock();
                _texture.dirty = false;
                _texture.width = _width;
                _texture.height = _height;
                _texture.format = GL_RGBA;
                _texture.buf.clear();
                _texture.buf.resize(_width * _height * 4, char(0xFF));
                glGetTexImage(GL_TEXTURE_2D, 0, _texture.format, GL_UNSIGNED_BYTE, _texture.buf.data());
                _texture.guard.unlock();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glBindTexture(GL_TEXTURE_2D, 0);
            glDeleteTextures(1, &TEX);
        }
        _cv.wait(lck);
    }
    glDeleteFramebuffers(1, &FBO);

    delete navMap;
    delete markerRender;
    delete mapRender;
}

NavMapThread::NavMapThread()
{
    _PlaneMarker_Lon = 0.0;
    _PlaneMarker_Lat = 0.0;
    _PlaneMarker_Hdg = 0.0;

    _PlaneMarker_Scale_X = 0.5;
    _PlaneMarker_Scale_Y = 0.5;

    _Map_Scale_X = 1.0;
    _Map_Scale_Y = 1.0;

    _width = 1;
    _height = 1;

    _working = false;
}

NavMapThread::~NavMapThread()
{
}

void NavMapThread::start(int width, int height)
{
    if (_working)
        return;

    _width = width;
    _height = height;

    _working = true;
    _th = std::thread(&NavMapThread::work, this);
}

void NavMapThread::stop()
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

void NavMapThread::setPlane(double lon, double lat, double hdg)
{
    std::unique_lock<std::mutex> lck(_mtx);

    _texture.guard.lock();
    _texture.dirty = true;
    _texture.guard.unlock();

    _PlaneMarker_Lon = lon;
    _PlaneMarker_Lat = lat;
    _PlaneMarker_Hdg = hdg;

    _cv.notify_all();
}

void NavMapThread::setPlaneScale(double scale_x, double scale_y)
{
    _texture.guard.lock();
    _texture.dirty = true;
    _texture.guard.unlock();

    std::unique_lock<std::mutex> lck(_mtx);

    _PlaneMarker_Scale_X = scale_x;
    _PlaneMarker_Scale_Y = scale_y;

    _cv.notify_all();
}

void NavMapThread::setScale(double scale_x, double scale_y)
{
    _texture.guard.lock();
    _texture.dirty = true;
    _texture.guard.unlock();

    std::unique_lock<std::mutex> lck(_mtx);

    _Map_Scale_X = scale_x;
    _Map_Scale_Y = scale_y;

    _cv.notify_all();
}
