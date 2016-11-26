#include "navmap.h"

NavMap::NavMap(IMapRender* mapRender, IMarkerRender* markerRender)
    :
    _mapRender(mapRender),
    _markerRender(markerRender)
{
    _PlaneMarker_Lon = 0.0;
    _PlaneMarker_Lat = 0.0;
    _PlaneMarker_Hdg = 0.0;

    _Map_Scale_X = 1.0;
    _Map_Scale_Y = 1.0;
}

void NavMap::setPlane(double lon, double lat, double hdg)
{
    _PlaneMarker_Lon = lon;
    _PlaneMarker_Lat = lat;
    _PlaneMarker_Hdg = hdg;

    _mapRender->prepare(_PlaneMarker_Lon, _PlaneMarker_Lat, _Map_Scale_X, _Map_Scale_Y);
    _markerRender->setRotation(_PlaneMarker_Hdg);
}

void NavMap::setPlaneScale(double scale_x, double scale_y)
{
    _markerRender->setScale(scale_x, scale_y);
}

void NavMap::setScale(double scale_x, double scale_y)
{
    _Map_Scale_X = scale_x;
    _Map_Scale_Y = scale_y;

    _mapRender->prepare(_PlaneMarker_Lon, _PlaneMarker_Lat, _Map_Scale_X, _Map_Scale_Y);
    _markerRender->setRotation(_PlaneMarker_Hdg);
}

void NavMap::render()
{
    _mapRender->render();
    _markerRender->render();
}
