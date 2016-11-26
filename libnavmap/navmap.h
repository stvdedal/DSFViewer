#ifndef NAVMAP_H_
#define NAVMAP_H_

#include <imap_render.h>
#include <imarker_render.h>

class NavMap
{
    IMapRender* _mapRender;
    IMarkerRender* _markerRender;

    double _PlaneMarker_Lon;
    double _PlaneMarker_Lat;
    double _PlaneMarker_Hdg;

    double _Map_Scale_X;
    double _Map_Scale_Y;

public:
    NavMap(IMapRender* mapRender, IMarkerRender* markerRender);
    
    void setPlane(double lon, double lat, double hdg);
    void setPlaneScale(double scale_x, double scale_y);
    void setScale(double scale_x, double scale_y);
    void render();
};

#endif
