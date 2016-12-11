#ifndef NAVMAP_H_
#define NAVMAP_H_

#include "inavmap.h"
#include <imap_render.h>
#include <imarker_render.h>

class NavMap : public INavMap
{
    IMapRender* _mapRender;
    IMarkerRender* _markerRender;

    double _map_lon;
    double _map_lat;

    double _map_scale_x;
    double _map_scale_y;

    double _marker_lon;
    double _marker_lat;
    double _marker_hdg;

    void translateMarker();

public:
    NavMap(IMapRender* mapRender, IMarkerRender* markerRender);
    
    virtual void setMap(double lon, double lat);
    virtual void setMapScale(double scale_x, double scale_y);

    virtual void setMarker(double lon, double lat, double hdg);
    virtual void setMarkerScale(double scale_x, double scale_y);

    virtual bool isMarkerOutOfBorder() const;

    void render();
};

#endif
