#include "dsf_map.h"
#include "utilities.h"

#include <glm/gtx/transform.hpp>

#include <iostream>
#include <cmath>


const GLchar* DsfMap::vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(position, 1.0);
}
)";

const GLchar* DsfMap::fragmentShaderSource = R"(
#version 330 core
out vec4 color;
void main()
{
    color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
)";

GLuint DsfMap::makeShader(GLenum shaderType, const char* shaderSource)
{
    GLint success;
    GLchar infoLog[512];
    GLuint shader;
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "[GL] ERROR: Shader compilation failure\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint DsfMap::makeShaderProgram(const GLchar* vshaderSource, const GLchar* fshaderSource)
{
    GLuint vertShader = makeShader(GL_VERTEX_SHADER, vshaderSource);
    GLuint fragShader = makeShader(GL_FRAGMENT_SHADER, fshaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        std::cerr << "[GL] ERROR: Shader linkage failure\n" << infoLog << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

DsfMap::DsfMap()
    : _shaderProgram(makeShaderProgram(vertexShaderSource, fragmentShaderSource))
{
}

DsfMap::~DsfMap()
{
    glDeleteProgram(_shaderProgram);
}

void DsfMap::prepare(double lon, double lat, double scale_x, double scale_y)
{
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
                dsf->load(dsf_lon, dsf_lat);
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

void DsfMap::render() const
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(_shaderProgram);
    GLint transformLoc = glGetUniformLocation(_shaderProgram, "transform");

    for (auto&& p : _objects)
    {
        if (p.second.age == 0)
            p.second.render(transformLoc);
    }
}
