#ifndef I_NAVMAP_H_
#define I_NAVMAP_H_

class INavMap
{
public:
    virtual ~INavMap() {}

    virtual void setPlane(double lon, double lat, double hdg) = 0;
    virtual void setPlaneScale(double scale_x, double scale_y) = 0;
    virtual void setScale(double scale_x, double scale_y) = 0;
    virtual void render() = 0;
};

#endif
