#include "navmap.h"
#include "navmap_impl.h"

NavMap::NavMap()
    :
    _impl(new NavMapImpl)
{
}

NavMap::~NavMap()
{
    delete _impl;
}

void NavMap::setTextureSize(int width, int height)
{
    _impl->setTextureSize(width, height);
}

void NavMap::setPlane(double lon, double lat, double hdg)
{
    _impl->setPlane(lon, lat, hdg);
}

void NavMap::setPlaneScale(double scale_x, double scale_y)
{
    _impl->setPlaneScale(scale_x, scale_y);
}

void NavMap::setScale(double scale_x, double scale_y)
{
    _impl->setScale(scale_x, scale_y);
}

void NavMap::render()
{
    _impl->render();
}
