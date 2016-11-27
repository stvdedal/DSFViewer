#ifndef NAVMAP_THREAD_H_
#define NAVMAP_THREAD_H_

#include "inavmap.h"
#include <imap_render.h>
#include <imarker_render.h>

#include <mutex>
#include <vector>

class NavMapThread : public INavMap
{
    std::mutex _mtx;

    double _PlaneMarker_Lon;
    double _PlaneMarker_Lat;
    double _PlaneMarker_Hdg;

    double _PlaneMarker_Scale_X;
    double _PlaneMarker_Scale_Y;

    double _Map_Scale_X;
    double _Map_Scale_Y;

    int _width, _height;

    void work();
    volatile bool _working;
    std::thread _th;
    std::condition_variable _cv;

protected:
    virtual IMarkerRender* createMarkerRender() = 0;
    virtual IMapRender*    createMapRender() = 0;

    struct FutureTexture
    {
        FutureTexture() : dirty(true), width(0), height(0) {}

        std::mutex guard;
        bool dirty;
        int width, height;
        int format;
        std::vector<char> buf;
    };
    FutureTexture _texture;

public:
    NavMapThread(const NavMapThread&) = delete;
    NavMapThread& operator=(const NavMapThread&) = delete;

    NavMapThread();
    ~NavMapThread();

    void start(int width, int height);
    void stop();

    void setPlane(double lon, double lat, double hdg);
    void setPlaneScale(double scale_x, double scale_y);
    void setScale(double scale_x, double scale_y);
};

#endif
