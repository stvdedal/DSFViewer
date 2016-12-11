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
                double map_lon = _map_lon;
                double map_lat = _map_lat;

                double map_scale_x = _map_scale_x;
                double map_scale_y = _map_scale_y;

                double marker_lon = _marker_lon;
                double marker_lat = _marker_lat;
                double marker_hdg = _marker_hdg;

                double marker_scale_x = _marker_scale_x;
                double marker_scale_y = _marker_scale_y;

                lck.unlock();

                glClearColor(1.0f, 1.0f, 1.0f, 0.3f);
                glClear(GL_COLOR_BUFFER_BIT);

                navMap->setMap(map_lon, map_lat);
                navMap->setMapScale(map_scale_x, map_scale_y);
                navMap->setMarker(marker_lon, marker_lat, marker_hdg);
                navMap->setMarkerScale(marker_scale_x, marker_scale_y);

                navMap->render();

                lck.lock();

                _isMarkerOutOfBorder = navMap->isMarkerOutOfBorder();

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
    _map_lon = 0.0;
    _map_lat = 0.0;

    _map_scale_x = 1.0;
    _map_scale_y = 1.0;

    _marker_lon = 0.0;
    _marker_lat = 0.0;
    _marker_hdg = 0.0;

    _marker_scale_x = 1.0;
    _marker_scale_y = 1.0;

    _width = 1;
    _height = 1;

    _working = false;

    _isMarkerOutOfBorder = false;
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

void NavMapThread::setMap(double lon, double lat)
{
    _texture.guard.lock();
    _texture.dirty = true;
    _texture.guard.unlock();

    std::unique_lock<std::mutex> lck(_mtx);

    _map_lon = lon;
    _map_lat = lat;

    _cv.notify_all();
}

void NavMapThread::setMapScale(double scale_x, double scale_y)
{
    _texture.guard.lock();
    _texture.dirty = true;
    _texture.guard.unlock();

    std::unique_lock<std::mutex> lck(_mtx);

    _map_scale_x = scale_x;
    _map_scale_y = scale_y;

    _cv.notify_all();
}

void NavMapThread::setMarker(double lon, double lat, double hdg)
{
    _texture.guard.lock();
    _texture.dirty = true;
    _texture.guard.unlock();

    std::unique_lock<std::mutex> lck(_mtx);

    _marker_lon = lon;
    _marker_lat = lat;
    _marker_hdg = hdg;

    _cv.notify_all();
}

void NavMapThread::setMarkerScale(double scale_x, double scale_y)
{
    _texture.guard.lock();
    _texture.dirty = true;
    _texture.guard.unlock();

    std::unique_lock<std::mutex> lck(_mtx);
 
    _marker_scale_x = scale_x;
    _marker_scale_y = scale_y;

    _cv.notify_all();
}

bool NavMapThread::isMarkerOutOfBorder() const
{
    std::unique_lock<std::mutex> lck(_mtx);
    bool isMarkerOutOfBorder = _isMarkerOutOfBorder;
    return isMarkerOutOfBorder;
}
