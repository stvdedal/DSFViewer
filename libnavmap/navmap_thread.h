#ifndef NAVMAP_THREAD_H_
#define NAVMAP_THREAD_H_

#include "inavmap.h"
#include <imap_render.h>
#include <imarker_render.h>

#include <mutex>
#include <vector>

class NavMapThread : public INavMap
{
    mutable std::mutex _mtx;

    double _map_lon;
    double _map_lat;

    double _map_scale_x;
    double _map_scale_y;

    double _marker_lon;
    double _marker_lat;
    double _marker_hdg;

    double _marker_scale_x;
    double _marker_scale_y;

    int _width, _height;

    bool _isMarkerOutOfBorder;

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

    virtual void setMap(double lon, double lat);
    virtual void setMapScale(double scale_x, double scale_y);

    virtual void setMarker(double lon, double lat, double hdg);
    virtual void setMarkerScale(double scale_x, double scale_y);

    virtual bool isMarkerOutOfBorder() const;
};

#endif
