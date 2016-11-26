#ifndef NAVMAP_H_
#define NAVMAP_H_

class NavMapImpl;

class NavMap
{
    NavMapImpl* _impl;

public:
    NavMap();
    NavMap(const NavMap&) = delete;
    NavMap& operator=(const NavMap&) = delete;
    ~NavMap();

    void setTextureSize(int width, int height);
    void setPlane(double lon, double lat, double hdg);
    void setPlaneScale(double scale_x, double scale_y);
    void setScale(double scale_x, double scale_y);
    void render();
};

#endif
