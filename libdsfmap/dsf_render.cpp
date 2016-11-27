#include "dsf_render.h"
#include "gl_check_error.h"
#include "utilities.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <cstdio>
#include <cstdlib>


#include <dsf_file.h>

// ---------------------------------------------------------------------
// DsfConstructor begin
// ---------------------------------------------------------------------

template<typename DsfObjectsContainer>
class DsfConstructor : public dsf::ICommand
{
    DsfObjectsContainer& _cont;

    GLfloat _west;
    GLfloat _east;
    GLfloat _north;
    GLfloat _south;

    std::vector<std::vector<double>> _data16;
    std::vector<unsigned int>        _depth16;
    std::vector<std::vector<double>> _data32;
    std::vector<unsigned int>        _depth32;
    unsigned int _currentPool;

    struct DrawData
    {
        std::vector<GLfloat> vertexData;
        std::vector<GLuint> elementData;
        GLuint dim;
        GLsizei lastWindingSize;
    } _polygonData_2d, _polygonData_3d;

    DrawElements makeDrawObject(DrawData* drawData);

    std::vector<std::vector<GLint>> _indices;
    GLint getIndex(unsigned int pool, unsigned int index);
    GLint putIndex(unsigned int pool, unsigned int index1, unsigned int index2);

public:
    DsfConstructor(DsfObjectsContainer& cont) : _cont(cont)
    {
        _polygonData_2d.dim = 2;
        _polygonData_3d.dim = 3;
    }

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

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::begin(const dsf::Atoms& atoms)
{
    get_properties(atoms, [this](auto&& p) {
        if (p.first == "sim/west")
            _west = static_cast<GLfloat>(atof(p.second.c_str()));
        if (p.first == "sim/east")
            _east = static_cast<GLfloat>(atof(p.second.c_str()));
        if (p.first == "sim/north")
            _north = static_cast<GLfloat>(atof(p.second.c_str()));
        if (p.first == "sim/south")
            _south = static_cast<GLfloat>(atof(p.second.c_str()));
    });

    get_plane_16(atoms, _data16, _depth16);
    get_plane_32(atoms, _data32, _depth32);
}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::end(const dsf::Atoms& atoms)
{
    _cont.push_back(makeDrawObject(&_polygonData_2d));
    _cont.push_back(makeDrawObject(&_polygonData_3d));
}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::polygon_begin(unsigned int definition, unsigned int param, unsigned int pool)
{
    _currentPool = pool;
}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::polygon_winding_begin()
{
    _polygonData_2d.lastWindingSize = 0;
    _polygonData_3d.lastWindingSize = 0;
}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::polygon_points(unsigned int first, unsigned int end)
{
    const double* p = _data16.at(_currentPool).data();
    unsigned int d = _depth16.at(_currentPool);

    if (d == 2)
    {
        for (unsigned int index = first; index < end; ++index)
        {
            GLint pointIndex = getIndex(_currentPool, index);
            if (pointIndex < 0)
            {
                // create point
                GLfloat x = static_cast<GLfloat>(p[index * d + 0]);
                GLfloat y = static_cast<GLfloat>(p[index * d + 1]);
                GLfloat nx = mapval(x, _west, _east, -1.0f, +1.0f);
                GLfloat ny = mapval(y, _south, _north, -1.0f, +1.0f);

                pointIndex = static_cast<GLint>(_polygonData_2d.vertexData.size() / d);
                _polygonData_2d.vertexData.push_back(nx);
                _polygonData_2d.vertexData.push_back(ny);
                putIndex(_currentPool, index, pointIndex);
            }
            _polygonData_2d.elementData.push_back(static_cast<GLuint>(pointIndex));
            ++_polygonData_2d.lastWindingSize;
        }
    }

    if (d == 3)
    {
        for (unsigned int index = first; index < end; ++index)
        {
            GLint pointIndex = getIndex(_currentPool, index);
            if (pointIndex < 0)
            {
                // create point
                GLfloat x = static_cast<GLfloat>(p[index * d + 0]);
                GLfloat y = static_cast<GLfloat>(p[index * d + 1]);
                GLfloat nx = mapval(x, _west, _east, -1.0f, +1.0f);
                GLfloat ny = mapval(y, _south, _north, -1.0f, +1.0f);
                GLfloat nz = 0;

                pointIndex = static_cast<GLint>(_polygonData_3d.vertexData.size() / d);
                _polygonData_3d.vertexData.push_back(nx);
                _polygonData_3d.vertexData.push_back(ny);
                _polygonData_3d.vertexData.push_back(nz);
                putIndex(_currentPool, index, pointIndex);
            }
            _polygonData_3d.elementData.push_back(static_cast<GLuint>(pointIndex));
            ++_polygonData_3d.lastWindingSize;
        }
    }
}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::polygon_winding_end()
{
    if (_polygonData_2d.lastWindingSize > 0)
        _polygonData_2d.elementData.push_back(std::numeric_limits<GLuint>::max());
    if (_polygonData_3d.lastWindingSize > 0)
        _polygonData_3d.elementData.push_back(std::numeric_limits<GLuint>::max());
}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::polygon_end()
{

}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::patch_begin(
    unsigned int definition,
    unsigned int patch_flags, double patch_LOD_nea, double patch_LOD_far,
    unsigned int pool)
{

}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::patch_primitive_begin(dsf::Primitive_type type)
{

}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::patch_primitive_vertex(unsigned int pool, unsigned int index)
{

}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::patch_primitive_end()
{

}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::patch_end()
{

}

template<typename DsfObjectsContainer>
void DsfConstructor<DsfObjectsContainer>::segment(
    unsigned int definition,
    unsigned int road_subtype,
    unsigned int pool, unsigned int begin, unsigned int end)
{

}

template<typename DsfObjectsContainer>
DrawElements DsfConstructor<DsfObjectsContainer>::makeDrawObject(DrawData* data)
{
    DrawElements draw;
    draw.loadData(data->vertexData.data(), data->vertexData.size() * sizeof(GLfloat));
    draw.loadElements(data->elementData.data(), data->elementData.size() * sizeof(GLuint));
    draw.setAttrib(0, data->dim, GL_FLOAT, data->dim * sizeof(GLfloat), 0);
    draw.setParam(GL_LINE_STRIP, GLsizei(data->elementData.size()), GL_UNSIGNED_INT, 0);
    return draw;
}

template<typename DsfObjectsContainer>
GLint DsfConstructor<DsfObjectsContainer>::getIndex(unsigned int pool, unsigned int index)
{
    if (pool >= _indices.size())
        _indices.resize(pool + 1, std::vector<GLint>());
    if (index >= _indices[pool].size())
        _indices[pool].resize(index + 1, -1);
    return _indices[pool][index];
}

template<typename DsfObjectsContainer>
GLint DsfConstructor<DsfObjectsContainer>::putIndex(unsigned int pool, unsigned int index1, unsigned int index2)
{
    if (pool >= _indices.size())
        _indices.resize(pool + 1, std::vector<GLint>());
    if (index1 >= _indices[pool].size())
        _indices[pool].resize(index1, -1);
    _indices[pool][index1] = index2;
    return index2;
}

// ---------------------------------------------------------------------
// DsfConstructor end
// ---------------------------------------------------------------------

DsfRender::DsfRender()
    :
    _rectangle(),
    _dsf_objects(),
    _transform()
{
    static const GLfloat data[] =
    {
        -1.0F, -1.0F,
        +1.0F, -1.0F,
        +1.0F, +1.0F,
        -1.0F, +1.0F,
    };
    static const GLuint elements[] = { 0, 1, 2, 3 };
    _rectangle.loadData(data, sizeof(data));
    _rectangle.loadElements(elements, sizeof(elements));
    _rectangle.setAttrib(0, 2, GL_FLOAT, 2 * sizeof(GLfloat), 0);
    _rectangle.setParam(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);

    age = -1;
}

DsfRender::DsfRender(DsfRender&& rhs)
    :
    _rectangle(std::move(rhs._rectangle)),
    _dsf_objects(std::move(rhs._dsf_objects)),
    _transform(std::move(rhs._transform))
{
}

DsfRender::~DsfRender()
{
}

bool DsfRender::loadFromFile(const char* file)
{
    bool result = false;

    _dsf_objects.clear();

    try
    {
        dsf::File dsfFile;
        if (dsfFile.open(file) &&
            dsfFile.header_ok() &&
            dsfFile.md5sum_ok())
        {
            dsfFile.prepare();

            DsfConstructor<std::list<DrawElements>> constructor(_dsf_objects);
            result = dsfFile.exec(&constructor);
        }
    }
    catch (...)
    {
        std::cerr << "Error: dsf exception" << std::endl;
    }
    if (!result)
    {
        std::cerr << "Error: dsf error" << std::endl;
    }

    return result;

#if 0
    system("del /Q tmp\\extracted_dsf\\*");

    const char* root_dir = "F:\\X-Plane 10\\Global Scenery\\X-Plane 10 Global Scenery\\Earth nav data";
    const char* extact_dir = "C:\\Windows\\Temp";
    char folder[64];
    snprintf(folder, sizeof(folder), "%+02d%+04d", lat - lat % 10, lon - lon % 10);
    char file[64];
    snprintf(file, sizeof(file), "%+02d%+04d.dsf", lat, lon);
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s\\%s\\%s", root_dir, folder, file);
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "\"\"%s\" x \"%s\" -o%s\" >NUL", "C:\\Program Files\\7-Zip\\7z.exe", fullpath, extact_dir);
    char extracted_file[64];
    snprintf(extracted_file, sizeof(extracted_file), "%s\\%s", extact_dir, file);

    int rv = system(cmd);
    if (rv == 0)
    {
        bool result = false;
        try
        {
            dsf::File dsfFile;
            if (dsfFile.open(extracted_file) &&
                dsfFile.header_ok() &&
                dsfFile.md5sum_ok())
            {
                dsfFile.prepare();

                DsfConstructor<std::list<DrawElements>> constructor(_dsf_objects);
                result = dsfFile.exec(&constructor);
            }
        }
        catch (...)
        {
            std::cerr << "Error: dsf exception" << std::endl;
        }
        if (!result)
        {
            std::cerr << "Error: dsf error" << std::endl;
        }
    }
    else
    {
        std::cerr << "Error: file \"" << fullpath << "\" not extracted (rv=" << rv << ")" << std::endl;
    }

    char delcmd[512];
    snprintf(delcmd, sizeof(delcmd), "del /Q %s", extracted_file);

    system(delcmd);
#endif
}

void DsfRender::setTransform(const glm::mat4& m)
{
    _transform = m;
}

void DsfRender::render(GLuint transformLoc) const
{
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(std::numeric_limits<GLuint>::max());

    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(_transform));
    //_rectangle();
    for (auto&& draw : _dsf_objects)
        draw();
}