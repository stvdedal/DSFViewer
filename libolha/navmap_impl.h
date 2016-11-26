#ifndef NAVMAP_IMPL_H_
#define NAVMAP_IMPL_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "dsf_map_thread.h"

class NavMapImpl
{
    static GLuint makeShader(GLenum shaderType, const GLchar* source);
    static GLuint makeProgram(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource);

    //
    // plane marker objects
    //
    static const GLchar* vertexShaderSourcePlaneMarker;
    static const GLchar* fragmentShaderSourcePlaneMarker;

    GLuint _programPlaneMarker;

    static const GLfloat vertexData_PlaneMarker[];

    GLuint _VAO_PlaneMarker, _VBO_PlaneMarker;

    GLfloat _planeMarker_lon, _planeMarker_lat;
    GLfloat _planeMarker_scale_x, _planeMarker_scale_y;
    GLfloat _planeMarker_rotate;
    glm::mat4 _transform_PlaneMarker;
    void setMatrix();


    //
    // textured map objects
    //

    static const GLchar* vertexShaderSourceTexturedMap;
    static const GLchar* fragmentShaderSourceTexturedMap;

    GLuint _programTexturedMap;

    static const GLfloat vertexData_TexturedMap[];

    GLuint _VAO_TexturedMap, _VBO_TexturedMap;
    GLuint _TEX_TexturedMap;

    GLfloat _map_scale_x, _map_scale_y;

    FutureTexture _mapTexture;
    MapTextureProvider _mapTextureProvider;

public:
    NavMapImpl(const NavMapImpl&) = delete;
    NavMapImpl& operator=(const NavMapImpl&) = delete;

    NavMapImpl();
    ~NavMapImpl();

    void setTextureSize(int width, int height);
    void setPlane(double lon, double lat, double hdg);
    void setPlaneScale(double scale_x, double scale_y);
    void setScale(double scale_x, double scale_y);
    void render();
};

#endif
