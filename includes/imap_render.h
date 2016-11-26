#ifndef I_MAP_RENDER_H_
#define I_MAP_RENDER_H_

class IMapRender
{
public:
    virtual ~IMapRender() {}

    virtual void prepare(double lon, double lat, double scale_x, double scale_y) = 0;
    virtual void render() const = 0;
};

#endif
