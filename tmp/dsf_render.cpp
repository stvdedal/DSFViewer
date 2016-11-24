#include "dsf_render.h"
#include <iostream>
#include <cstdlib>

const char* DsfRender::vertexShaderStr =
"#version 330 core"                                                 "\n"
"layout (location = 0) in vec3 position;"                           "\n"
"void main()"                                                       "\n"
"{"                                                                 "\n"
"    gl_Position = vec4(position.x, position.y, position.z, 1.0);"  "\n"
"}"                                                                 "\n";

const char* DsfRender::fragmentShaderStr =
"#version 330 core"                                                 "\n"
"out vec4 color;"                                                   "\n"
"void main()"                                                       "\n"
"{"                                                                 "\n"
"    color = vec4(0.0f, 0.0f, 0.0f, 1.0f);"                         "\n"
"}"                                                                 "\n";

double DsfRender::mapv(double x, double x1, double x2, double v1, double v2)
{
    double dx = x2 - x1;
    double dv = v2 - v1;
    double d = dv / dx;
    return ((x - x1) * d) + v1;
}

void DsfRender::checkGLErrors(int atLine)
{
    for (GLenum err; (err = glGetError()) != GL_NO_ERROR; )
    {
        std::cerr << "OpenGL Error: " << err << " 0x" << std::hex << err << std::dec << " at line " << atLine << std::endl;
        throw GlErrorException();
    }
}

DsfRender::DsfRender()
    :
    _shaderProgram(0),
    _west(), _east(), _north(), _south()
{
}

GLuint DsfRender::makeShader(GLenum shaderType, const char* str)
{
    GLint success;
    GLchar infoLog[512];
    GLuint shader;

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &str, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "ERROR: SHADER COMPILATION FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint DsfRender::makeShaderProgram()
{
    GLuint vertShader = makeShader(GL_VERTEX_SHADER, vertexShaderStr);
    GLuint fragShader = makeShader(GL_FRAGMENT_SHADER, fragmentShaderStr);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    glBindAttribLocation(program, 0, "position");

    GLint success;
    GLchar infoLog[512];

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
        std::cerr << "ERROR: SHADER LINK FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

void DsfRender::setup()
{
    _shaderProgram = makeShaderProgram();
    checkGLErrors(__LINE__);
}

void DsfRender::render()
{
    checkGLErrors(__LINE__);

    // Clear the colorbuffer
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    checkGLErrors(__LINE__);

    glUseProgram(_shaderProgram);
    glBindVertexArray(_VAO_2d);
    glDrawElements(GL_POINTS, _elements_2d.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindVertexArray(_VAO_3d);
    glDrawElements(GL_POINTS, _elements_3d.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    checkGLErrors(__LINE__);
}

// ----------------------------------------------
// BEGIN dsf::ICommand
// ----------------------------------------------

void DsfRender::begin(const dsf::Atoms& atoms)
{
    get_plane_16(atoms, _data16, _depth16);
    get_plane_32(atoms, _data32, _depth32);

    get_properties(atoms, [this](auto&& p) {
        if (p.first == "sim/west")
            _west = atof(p.second.c_str());
        if (p.first == "sim/east")
            _east = atof(p.second.c_str());
        if (p.first == "sim/north")
            _north = atof(p.second.c_str());
        if (p.first == "sim/south")
            _south = atof(p.second.c_str());
    });
}

void DsfRender::end(const dsf::Atoms& atoms)
{
    // ----  2d -----------------------------------------
    glGenVertexArrays(1, &_VAO_2d);
    glGenBuffers(1, &_VBO_2d);
    glGenBuffers(1, &_EBO_2d);

    glBindVertexArray(_VAO_2d);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO_2d);
    glBufferData(GL_ARRAY_BUFFER, _vertices_2d.size() * sizeof(GLfloat), _vertices_2d.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO_2d);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _elements_2d.size() * sizeof(GLuint), _elements_2d.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ----  3d -----------------------------------------
    glGenVertexArrays(1, &_VAO_3d);
    glGenBuffers(1, &_VBO_3d);
    glGenBuffers(1, &_EBO_3d);

    glBindVertexArray(_VAO_3d);

    glBindBuffer(GL_ARRAY_BUFFER, _VBO_3d);
    glBufferData(GL_ARRAY_BUFFER, _vertices_3d.size() * sizeof(GLfloat), _vertices_3d.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO_3d);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _elements_3d.size() * sizeof(GLuint), _elements_3d.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    checkGLErrors(__LINE__);
}

void DsfRender::polygon_begin(unsigned int definition, unsigned int param, unsigned int pool)
{
    _currentPool = pool;
}

void DsfRender::polygon_winding_begin()
{

}

int ddd = 0;

void DsfRender::polygon_points(unsigned int first, unsigned int end)
{
    const double* p = _data16.at(_currentPool).data();
    unsigned int d = _depth16.at(_currentPool);

    if (d == 2)
    {
        for (unsigned int index = first; index < end; ++index)
        {
            double x = p[index * d + 0];
            double y = p[index * d + 1];

            if (!(x >= _west && x <= _east))
            {
                std::cerr << "x " << x << " out of range" << std::endl;
            }
            if (!(y >= _south && y <= _north))
            {
                std::cerr << "y " << y << " out of range" << std::endl;
            }

            _vertices_2d.push_back(mapv(x, _west, _east));
            //std::cerr << _vertices.back() << " ";
            _vertices_2d.push_back(mapv(y, _south, _north));
            //std::cerr << _vertices.back() << std::endl;

            _elements_2d.push_back(_elements_2d.size());
        }
    }

    if (d == 3)
    {
        for (unsigned int index = first; index < end; ++index)
        {
            double x = p[index * d + 0];
            double y = p[index * d + 1];

            if (!(x >= _west && x <= _east))
            {
                std::cerr << "x " << x << " out of range" << std::endl;
            }
            if (!(y >= _south && y <= _north))
            {
                std::cerr << "y " << y << " out of range" << std::endl;
            }

            _vertices_3d.push_back(mapv(x, _west, _east));
            _vertices_3d.push_back(mapv(y, _south, _north));
            _vertices_3d.push_back(0);

            _elements_3d.push_back(_elements_3d.size());
        }
    }
}

void DsfRender::polygon_winding_end()
{

}

void DsfRender::polygon_end()
{

    //throw BreakException();
}

void DsfRender::patch_begin(
    unsigned int definition,
    unsigned int patch_flags, double patch_LOD_nea, double patch_LOD_far,
    unsigned int pool)
{
    _currentPool = pool;
}

void DsfRender::patch_primitive_begin(dsf::Primitive_type type)
{

}

void DsfRender::patch_primitive_vertex(unsigned int pool, unsigned int index)
{

}

void DsfRender::patch_primitive_end()
{

}

void DsfRender::patch_end()
{

}

void DsfRender::segment(
    unsigned int definition,
    unsigned int road_subtype,
    unsigned int pool, unsigned int begin, unsigned int end)
{
    _currentPool = pool;
}

// ----------------------------------------------
// END dsf::ICommand
// ----------------------------------------------

