#ifndef DSF_MAP_H_
#define DSF_MAP_H_

#include <imap_render.h>
#include "dsf_render.h"
#include <map>

class DsfMap : public IMapRender
{
    static const GLchar* vertexShaderSource;
    static const GLchar* fragmentShaderSource;

    GLuint _shaderProgram;
    static GLuint makeShader(GLenum shaderType, const char* shaderSource);
    static GLuint makeShaderProgram(const GLchar* vshaderSource, const GLchar* fshaderSource);

    // map longitude and latitude to DSF
    std::map<std::pair<int,int>, DsfRender> _objects;

public:
    DsfMap();
    ~DsfMap();

    void prepare(double lon, double lat, double scale_x, double scale_y);
    void render() const;
};

#endif
