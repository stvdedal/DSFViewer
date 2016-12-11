#include "navmap.h"
#include <iostream>

NavMap::NavMap(IMapRender* mapRender, IMarkerRender* markerRender)
    :
    _mapRender(mapRender),
    _markerRender(markerRender)
{
    _map_lon = 0.0;
    _map_lat = 0.0;

    _map_scale_x = 1.0;
    _map_scale_y = 1.0;

    _marker_lon = 0.0;
    _marker_lat = 0.0;
    _marker_hdg = 0.0;
}

void NavMap::translateMarker()
{
    double x = (_marker_lon - _map_lon) / (_map_scale_x / 2.0);
    double y = (_marker_lat - _map_lat) / (_map_scale_y / 2.0);

    if (x < -1.0)
        x = -1.0;
    if (x > +1.0)
        x = +1.0;

    if (y < -1.0)
        y = -1.0;
    if (y > +1.0)
        y = +1.0;

    _markerRender->setTranslate(x, y);
}

void NavMap::setMap(double lon, double lat)
{
    _map_lon = lon;
    _map_lat = lat;

    _mapRender->prepare(_map_lon, _map_lat, _map_scale_x, _map_scale_y);
    translateMarker();
}

void NavMap::setMapScale(double scale_x, double scale_y)
{
    _map_scale_x = scale_x;
    _map_scale_y = scale_y;

    _mapRender->prepare(_map_lon, _map_lat, _map_scale_x, _map_scale_y);
    translateMarker();
}

void NavMap::setMarker(double lon, double lat, double hdg)
{
    _marker_lon = lon;
    _marker_lat = lat;
    _marker_hdg = hdg;

    translateMarker();
    _markerRender->setRotation(_marker_hdg);
}

void NavMap::setMarkerScale(double scale_x, double scale_y)
{
    _markerRender->setScale(scale_x, scale_y);
}

bool NavMap::isMarkerOutOfBorder() const
{
    double x = (_marker_lon - _map_lon) / (_map_scale_x / 2.0);
    double y = (_marker_lat - _map_lat) / (_map_scale_y / 2.0);

    return abs(x) > 1.0 || abs(y) > 1.0;
}

void NavMap::render()
{
    _mapRender->render();
    _markerRender->render();
}
