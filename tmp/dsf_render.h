#ifndef DSF_RENDER_H_
#define DSF_RENDER_H_

#include <GL/glew.h>
#include "dsf.h"
#include <map>

class DsfRender : public dsf::ICommand
{
    static const char* vertexShaderStr;
    static const char* fragmentShaderStr;

    GLuint _shaderProgram;

    GLuint _VAO_2d, _VBO_2d, _EBO_2d;
    std::vector<GLfloat> _vertices_2d;
    std::vector<GLuint> _elements_2d;

    GLuint _VAO_3d, _VBO_3d, _EBO_3d;
    std::vector<GLfloat> _vertices_3d;
    std::vector<GLuint> _elements_3d;

    double _west;
    double _east;
    double _north;
    double _south;

    std::vector<std::vector<double>> _data16;
    std::vector<unsigned int>        _depth16;
    std::vector<std::vector<double>> _data32;
    std::vector<unsigned int>        _depth32;

    unsigned int _currentPool;

    static double mapv(double x, double x1, double x2, double v1 = -1.0, double v2 = 1.0);

    static void checkGLErrors(int atLine);
    static GLuint makeShader(GLenum shaderType, const char*);
    static GLuint makeShaderProgram();
    

public:
    struct BreakException {};
    struct GlErrorException {};

    DsfRender();

    void setup();
    void render();

    // ----------------------------------------------
    // dsf::ICommand
    // ----------------------------------------------
    virtual void begin(const dsf::Atoms& atoms);
    virtual void end(const dsf::Atoms& atoms);

    virtual void polygon_begin(unsigned int definition, unsigned int param, unsigned int pool);
    virtual void polygon_winding_begin();
    virtual void polygon_points(unsigned int first, unsigned int end);
    virtual void polygon_winding_end();
    virtual void polygon_end();

    virtual void patch_begin(
        unsigned int definition,
        unsigned int patch_flags, double patch_LOD_nea, double patch_LOD_far,
        unsigned int pool);
    virtual void patch_primitive_begin(dsf::Primitive_type type);
    virtual void patch_primitive_vertex(unsigned int pool, unsigned int index);
    virtual void patch_primitive_end();
    virtual void patch_end();

    virtual void segment(
        unsigned int definition,
        unsigned int road_subtype,
        unsigned int pool, unsigned int begin, unsigned int end);
    // ----------------------------------------------
};

#endif
