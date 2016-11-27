#include "dsf_map.h"
#include "dsf_map_impl.h"

DsfMap::DsfMap()
    :
    _impl(new DsfMapImpl)
{
}

DsfMap::~DsfMap()
{
    delete _impl;
}

void DsfMap::setDsfDirectory(const std::string& dir)
{
    _impl->setDsfDirectory(dir);
}

void DsfMap::setTmpDirectory(const std::string& dir)
{
    _impl->setTmpDirectory(dir);
}

void DsfMap::prepare(double lon, double lat, double scale_x, double scale_y)
{
    _impl->prepare(lon, lat, scale_x, scale_y);
}

void DsfMap::render() const
{
    _impl->render();
}
