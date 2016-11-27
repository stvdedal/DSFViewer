#ifndef NAVMAP_SIMPLE_MARKER_H_
#define NAVMAP_SIMPLE_MARKER_H_

#include <imarker_render.h>

class SimpleMarkerImpl;

class SimpleMarker : public IMarkerRender
{
    SimpleMarkerImpl* _impl;
public:
    SimpleMarker();
    ~SimpleMarker();

    virtual void setScale(double scale_x, double scale_y);
    virtual void setRotation(double angle);
    virtual void render();
};

#endif
