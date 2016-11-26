#include "navmap_impl.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const GLchar* NavMapImpl::vertexShaderSourceTexturedMap = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex;
out vec2 texCoord;
void main()
{
    gl_Position = vec4(position, 1.0);
    texCoord = tex;
}
)";

const GLchar* NavMapImpl::fragmentShaderSourceTexturedMap = R"(
#version 330 core
in vec2 texCoord;
out vec4 color;
uniform sampler2D texSampler;
void main()
{
    color = texture(texSampler, texCoord);
}
)";

const GLchar* NavMapImpl::vertexShaderSourcePlaneMarker = R"(
#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 transform;
void main()
{
    gl_Position = transform * vec4(position, 1.0);
}
)";

const GLchar* NavMapImpl::fragmentShaderSourcePlaneMarker = R"(
#version 330 core
out vec4 color;
void main()
{
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)";

const GLfloat NavMapImpl::vertexData_PlaneMarker[] =
{
    // triangle
    -0.5F, -0.5F, 0.0F,
    +0.5F, -0.5F, 0.0F,
    +0.0F, +1.0F, 0.0F,
};

const GLfloat NavMapImpl::vertexData_TexturedMap[] =
{
    // position            texture
    -1.0F, -1.0F, 0.0F,   0.0F, 0.0F,
    +1.0F, -1.0F, 0.0F,   1.0F, 0.0F,
    +1.0F, +1.0F, 0.0F,   1.0F, 1.0F,
    -1.0F, +1.0F, 0.0F,   0.0F, 1.0F
};

GLuint NavMapImpl::makeShader(GLenum shaderType, const GLchar* source)
{
    GLint success;
    GLchar infoLog[512];
    GLuint shader;

    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "ERROR: SHADER COMPILATION FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint NavMapImpl::makeProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
{
    GLuint vertShader = makeShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragShader = makeShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

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

void NavMapImpl::setMatrix()
{
    glm::mat4 transform;
    transform = glm::rotate(transform, glm::radians(-_planeMarker_rotate), glm::vec3(0.0, 0.0, 1.0));
    transform = glm::scale(transform, glm::vec3(_planeMarker_scale_x, _planeMarker_scale_y, 1.0f));
    _transform_PlaneMarker = transform;
}

NavMapImpl::NavMapImpl()
{
    _planeMarker_lon = 0.0f;
    _planeMarker_lat = 0.0f;
    _planeMarker_scale_x = 1.0f;
    _planeMarker_scale_y = 1.0f;
    _planeMarker_rotate = 0.0f;

    _map_scale_x = 1.0f;
    _map_scale_y = 1.0f;

    _programTexturedMap = makeProgram(vertexShaderSourceTexturedMap, fragmentShaderSourceTexturedMap);
    _programPlaneMarker = makeProgram(vertexShaderSourcePlaneMarker, fragmentShaderSourcePlaneMarker);

    //
    // plane marker
    //
    glGenVertexArrays(1, &_VAO_PlaneMarker);
    glGenBuffers(1, &_VBO_PlaneMarker);
    glBindVertexArray(_VAO_PlaneMarker);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO_PlaneMarker);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData_PlaneMarker), vertexData_PlaneMarker, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //
    // textured map
    //
    glGenVertexArrays(1, &_VAO_TexturedMap);
    glGenBuffers(1, &_VBO_TexturedMap);
    glBindVertexArray(_VAO_TexturedMap);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO_TexturedMap);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData_TexturedMap), vertexData_TexturedMap, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    glGenTextures(1, &_TEX_TexturedMap);
    glBindTexture(GL_TEXTURE_2D, _TEX_TexturedMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

}

NavMapImpl::~NavMapImpl()
{
    glDeleteTextures(1, &_TEX_TexturedMap);

    glDeleteBuffers(1, &_VBO_TexturedMap);
    glDeleteVertexArrays(1, &_VAO_TexturedMap);

    glDeleteBuffers(1, &_VBO_PlaneMarker);
    glDeleteVertexArrays(1, &_VAO_PlaneMarker);

    glDeleteProgram(_programTexturedMap);
    glDeleteProgram(_programPlaneMarker);
}

void NavMapImpl::setTextureSize(int width, int height)
{
    _mapTextureProvider.stop();
    _mapTextureProvider.start(GLuint(width), GLuint(height));
}

void NavMapImpl::setPlane(double lon, double lat, double hdg)
{
    _planeMarker_lon = GLfloat(lon);
    _planeMarker_lat = GLfloat(lat);
    _planeMarker_rotate = GLfloat(hdg);

    setMatrix();

    _mapTextureProvider.get(_planeMarker_lon, _planeMarker_lat, _map_scale_x, _map_scale_y, &_mapTexture);
}

void NavMapImpl::setPlaneScale(double scale_x, double scale_y)
{
    _planeMarker_scale_x = GLfloat(scale_x);
    _planeMarker_scale_y = GLfloat(scale_y);
    setMatrix();
}

void NavMapImpl::setScale(double scale_x, double scale_y)
{
    _map_scale_x = GLfloat(scale_x);
    _map_scale_y = GLfloat(scale_y);

    _mapTextureProvider.get(_planeMarker_lon, _planeMarker_lat, _map_scale_x, _map_scale_y, &_mapTexture);
}

void NavMapImpl::render()
{
    //
    // textured map
    //
    glUseProgram(_programTexturedMap);

    glBindTexture(GL_TEXTURE_2D, _TEX_TexturedMap);
    _mapTexture.load();

    glBindVertexArray(_VAO_TexturedMap);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //
    // plane marker
    //
    glUseProgram(_programPlaneMarker);

    GLint transformLoc = glGetUniformLocation(_programPlaneMarker, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(_transform_PlaneMarker));

    glBindVertexArray(_VAO_PlaneMarker);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
