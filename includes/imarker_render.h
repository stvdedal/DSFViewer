#ifndef I_MARKER_RENDER_H_
#define I_MARKER_RENDER_H_

class IMarkerRender
{
public:
    virtual ~IMarkerRender() {}

    virtual void setTranslate(double x, double y) = 0;
    virtual void setScale(double scale_x, double scale_y) = 0;
    virtual void setRotation(double angle) = 0;
    virtual void render() = 0;
};

#endif
