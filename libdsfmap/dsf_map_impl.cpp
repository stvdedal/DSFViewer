#include "dsf_map_impl.h"
#include "utilities.h"
#include <shader_maker.h>

#include <glm/gtx/transform.hpp>

#include <iostream>
#include <cmath>
#include <cstdio>

const GLchar* DsfMapImpl::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(position, 1.0);
}
)";

const GLchar* DsfMapImpl::fragmentShaderSource = R"(
#version 330 core
out vec4 color;
void main()
{
    color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
)";

DsfMapImpl::DsfMapImpl()
    : _shaderProgram(makeProgram(vertexShaderSource, fragmentShaderSource))
{
}

DsfMapImpl::~DsfMapImpl()
{
    glDeleteProgram(_shaderProgram);
}

void DsfMapImpl::loadDsf(DsfRender* dsf, int lon, int lat)
{
    char dsfDir[64];
    snprintf(dsfDir, sizeof(dsfDir), "%+02d%+04d", lat - lat % 10, lon - lon % 10);
    char dsfFile[64];
    snprintf(dsfFile, sizeof(dsfFile), "%+02d%+04d.dsf", lat, lon);
    char fullpath[512];
    snprintf(fullpath, sizeof(fullpath), "%s\\%s\\%s", _dsf_directory.c_str(), dsfDir, dsfFile);
    char extracted_file[64];
    snprintf(extracted_file, sizeof(extracted_file), "%s\\%s", _tmp_directory.c_str(), dsfFile);

    if (extract(fullpath, _tmp_directory))
    {
        bool result = dsf->loadFromFile(extracted_file);
        std::cerr << extracted_file << " result="<< result << std::endl;
    }
    else
    {
        std::cerr << "[DsfMap] Error: file \"" << extract << "\" not extracted" << std::endl;
    }
    std::remove(extracted_file);
}

bool DsfMapImpl::extract(const std::string& packedFile, const std::string& outputDir)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "\"\"%s\" x \"%s\" -o%s\" >NUL",
        "C:\\Program Files\\7-Zip\\7z.exe",
        packedFile.c_str(),
        outputDir.c_str());
    return system(cmd) == 0;
}

void DsfMapImpl::setDsfDirectory(const std::string& dir)
{
    _dsf_directory = dir;
}

void DsfMapImpl::setTmpDirectory(const std::string& dir)
{
    _tmp_directory = dir;
}

void DsfMapImpl::prepare(double lon, double lat, double scale_x, double scale_y)
{
    if (_dsf_directory.empty() || _tmp_directory.empty())
    {
        std::cerr << "[DsfMap] ERROR: directories are empty" << std::endl;
        return;
    }

    GLfloat left_lon = GLfloat(lon - scale_x / 2.0);    // longitude of the -1.0 NDC.X
    GLfloat bottom_lat = GLfloat(lat - scale_y / 2.0);  // latitude of the -1.0 NDC.Y
    GLfloat right_lon = GLfloat(lon + scale_x / 2.0);   // longitude of the +1.0 NDC.X
    GLfloat top_lat = GLfloat(lat + scale_y / 2.0);     // latitude of the +1.0 NDC.Y

    GLfloat left_lon_integ;
    modf(left_lon, &left_lon_integ);

    GLfloat bottom_lat_integ;
    modf(bottom_lat, &bottom_lat_integ);

    GLfloat start_x = mapval(left_lon_integ, left_lon, right_lon, -1.0f, +1.0f);
    GLfloat start_y = mapval(bottom_lat_integ, bottom_lat, top_lat, -1.0f, +1.0f);
    GLfloat step_x = GLfloat(2.0 / scale_x);    // width of one dsf in NDC
    GLfloat step_y = GLfloat(2.0 / scale_y);    // height of one dsf in NDC

    for (auto&& p : _objects)
        ++p.second.age;

    int dsf_lat = int(bottom_lat);
    for (GLfloat y = start_y; y <= 1.0f; y += step_y)
    {
        int dsf_lon = int(left_lon);
        for (GLfloat x = start_x; x <= 1.0f; x += step_x)
        {
            DsfRender* dsf = &_objects[std::make_pair(dsf_lon, dsf_lat)];

            if (dsf->age < 0)
            {
                loadDsf(dsf, dsf_lon, dsf_lat);
                //dsf->load(dsf_lon, dsf_lat);
            }
            dsf->age = 0;

            glm::mat4 transform;
            transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
            transform = glm::scale(transform, glm::vec3(1.0f / scale_x, 1.0f / scale_y, 0.0f));
            transform = glm::translate(transform, glm::vec3(1.0f, 1.0f, 0.0f));
            dsf->setTransform(transform);

            dsf_lon += 1; // dfs has 1 degree width
        }

        dsf_lat += 1; // dfs has 1 degree height
    }

    for (auto it = _objects.begin(); it != _objects.end(); )
    {
        if (it->second.age > 10)
            _objects.erase(it++);
        else
            ++it;
    }
}

void DsfMapImpl::render() const
{
    glUseProgram(_shaderProgram);
    GLint transformLoc = glGetUniformLocation(_shaderProgram, "transform");

    for (auto&& p : _objects)
    {
        if (p.second.age == 0)
            p.second.render(transformLoc);
    }
}
