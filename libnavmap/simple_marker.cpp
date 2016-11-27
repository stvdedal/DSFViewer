#include "simple_marker.h"
#include "simple_marker_impl.h"

SimpleMarker::SimpleMarker()
    :
    _impl(new SimpleMarkerImpl)
{
}

SimpleMarker::~SimpleMarker()
{
    delete _impl;
}

void SimpleMarker::setScale(double scale_x, double scale_y)
{
    _impl->setScale(scale_x, scale_y);
}

void SimpleMarker::setRotation(double angle)
{
    _impl->setRotation(angle);
}

void SimpleMarker::render()
{
    _impl->render();
}
