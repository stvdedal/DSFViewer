#ifndef DSF_MAP_IMPL_H_
#define DSF_MAP_IMPL_H_

#include <imap_render.h>
#include "dsf_render.h"
#include <map>
#include <string>

class DsfMapImpl
{
    static const GLchar* vertexShaderSource;
    static const GLchar* fragmentShaderSource;

    GLuint _shaderProgram;

    // map longitude and latitude to DSF
    std::map<std::pair<int, int>, DsfRender> _objects;

    std::string _dsf_directory;

    void loadDsf(DsfRender* dsf, int lon, int lat);

public:
    DsfMapImpl();
    ~DsfMapImpl();

    void setDsfDirectory(const std::string& dir);
    void setTmpDirectory(const std::string& dir);

    void prepare(double lon, double lat, double scale_x, double scale_y);
    void render() const;
};

#endif
