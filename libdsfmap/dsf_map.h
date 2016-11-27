#ifndef DSF_MAP_H_
#define DSF_MAP_H_

#include <imap_render.h>
#include <string>

class DsfMapImpl;

class DsfMap : public IMapRender
{
    DsfMapImpl* _impl;
public:
    DsfMap();
    ~DsfMap();

    void setDsfDirectory(const std::string& dir);
    void setTmpDirectory(const std::string& dir);

    void prepare(double lon, double lat, double scale_x, double scale_y);
    void render() const;
};

#endif
