#ifndef I_NAVMAP_H_
#define I_NAVMAP_H_

class INavMap
{
public:
    virtual ~INavMap() {}

    virtual void setMap(double lon, double lat) = 0;
    virtual void setMapScale(double scale_x, double scale_y) = 0;

    virtual void setMarker(double lon, double lat, double hdg) = 0;
    virtual void setMarkerScale(double scale_x, double scale_y) = 0;

    virtual bool isMarkerOutOfBorder() const = 0;

    virtual void render() = 0;
};

#endif
