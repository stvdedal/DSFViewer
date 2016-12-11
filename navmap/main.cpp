#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <stdlib.h>

#include <dsf_map.h>
#include <simple_marker.h>

#include "shader_maker.h"
#include "drawable_object.h"
#include <gl_check_error.h>

static const char* dsfRootDir = "F:\\X-Plane 10\\Global Scenery\\X-Plane 10 Global Scenery\\Earth nav data";
static const char* tmpDir = "C:\\Windows\\Temp";

//
// uncomment to use NavMapThread
//   comment to use NavMap
//
#define TEST_NAVMAP_THREAD

#if defined(TEST_NAVMAP_THREAD)
#include <navmap_thread.h>
class DsfMapSimpleMarkerNavMapThread : public NavMapThread
{
    IMarkerRender* createMarkerRender() { return new SimpleMarker; }
    IMapRender*    createMapRender()
    {
        DsfMap* dsfMap = new DsfMap;
        dsfMap->setDsfDirectory(dsfRootDir);
        dsfMap->setTmpDirectory(tmpDir);
        return dsfMap;
    }

    static const GLchar* vertexShaderSource;
    static const GLchar* fragmentShaderSource;

    GLuint _program;
    DrawElements _rect;
    GLuint _TEX;
public:
    DsfMapSimpleMarkerNavMapThread();
    ~DsfMapSimpleMarkerNavMapThread();
    void render();
};

const GLchar* DsfMapSimpleMarkerNavMapThread::vertexShaderSource = R"(
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

const GLchar* DsfMapSimpleMarkerNavMapThread::fragmentShaderSource = R"(
#version 330 core
in vec2 texCoord;
out vec4 color;
uniform sampler2D texSampler;
void main()
{
    color = texture(texSampler, texCoord);
}
)";

DsfMapSimpleMarkerNavMapThread::DsfMapSimpleMarkerNavMapThread()
{
    _program = makeProgram(vertexShaderSource, fragmentShaderSource);

    static const GLfloat vertexData[] =
    {
        // position            texture
        -1.0F, -1.0F, 0.0F,   0.0F, 0.0F,
        +1.0F, -1.0F, 0.0F,   1.0F, 0.0F,
        +1.0F, +1.0F, 0.0F,   1.0F, 1.0F,
        -1.0F, +1.0F, 0.0F,   0.0F, 1.0F
    };

    static const GLuint vertexElements[] = { 0, 1, 2, 3 };

    _rect.loadData(vertexData, sizeof(vertexData));
    _rect.loadElements(vertexElements, sizeof(vertexElements));
    _rect.setAttrib(0, 3, GL_FLOAT, 5 * sizeof(GLfloat), 0);
    _rect.setAttrib(1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
    _rect.setParam(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);

    glGenTextures(1, &_TEX);
    glBindTexture(GL_TEXTURE_2D, _TEX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

DsfMapSimpleMarkerNavMapThread::~DsfMapSimpleMarkerNavMapThread()
{
    glDeleteTextures(1, &_TEX);
    glDeleteProgram(_program);
}

void DsfMapSimpleMarkerNavMapThread::render()
{
    glUseProgram(_program);

    glBindTexture(GL_TEXTURE_2D, _TEX);

    _texture.guard.lock();
    if (!_texture.dirty)
        glTexImage2D(GL_TEXTURE_2D, 0, _texture.format, _texture.width, _texture.height, 0, _texture.format, GL_UNSIGNED_BYTE, _texture.buf.data());
    _texture.guard.unlock();

    _rect();

    glBindTexture(GL_TEXTURE_2D, 0);
}
#else
#include <navmap.h>
#endif


INavMap* navMap;

static double mapLon = 46.0;
static double mapLat = 51.2;
static double mapScaleX = 1.0;
static double mapScaleY = 1.0;

static double planeLon = 46.0;
static double planeLat = 51.2;
static double planeHdg = 45.0;

static void setWindowTitle(GLFWwindow* window)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "Nav Map. Map: lon=%f lat=%f sc_x=%f sc_y=%f. Plane: lon=%f lat=%f hdg=%f",
        mapLon, mapLat, mapScaleX, mapScaleY,
        planeLon, planeLat, planeHdg
    );
    glfwSetWindowTitle(window, buf);
}

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "[GLFW] ERROR: " << error << ". " << description << std::endl;
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_REPEAT || action == GLFW_PRESS)
    {
        const double coordStep = 0.005;
        const double scaleStep = 0.05;

        switch (key)
        {
        case GLFW_KEY_LEFT:
            if (mods == 0)
            {
                mapLon -= coordStep;
                navMap->setMap(mapLon, mapLat);
            }
            else if (mods & GLFW_MOD_SHIFT)
            {
                planeLon -= coordStep;
                navMap->setMarker(planeLon, planeLat, planeHdg);
            }
            break;
        case GLFW_KEY_RIGHT:
            if (mods == 0)
            {
                mapLon += coordStep;
                navMap->setMap(mapLon, mapLat);
            }
            else if (mods & GLFW_MOD_SHIFT)
            {
                planeLon += coordStep;
                navMap->setMarker(planeLon, planeLat, planeHdg);
            }
            break;
        case GLFW_KEY_UP:
            if (mods == 0)
            {
                mapLat += coordStep;
                navMap->setMap(mapLon, mapLat);
            }
            else if (mods & GLFW_MOD_SHIFT)
            {
                planeLat += coordStep;
                navMap->setMarker(planeLon, planeLat, planeHdg);
            }
            break;
        case GLFW_KEY_DOWN:
            if (mods == 0)
            {
                mapLat -= coordStep;
                navMap->setMap(mapLon, mapLat);
            }
            else if (mods & GLFW_MOD_SHIFT)
            {
                planeLat -= coordStep;
                navMap->setMarker(planeLon, planeLat, planeHdg);
            }
            break;
        case GLFW_KEY_INSERT:
            mapScaleX += scaleStep;
            mapScaleY += scaleStep;
            navMap->setMapScale(mapScaleX, mapScaleY);
            break;
        case GLFW_KEY_DELETE:
            mapScaleX -= scaleStep;
            mapScaleY -= scaleStep;
            if (mapScaleX < scaleStep)
                mapScaleX = scaleStep;
            if (mapScaleY < scaleStep)
                mapScaleY = scaleStep;
            navMap->setMapScale(mapScaleX, mapScaleY);
            break;
        case GLFW_KEY_PAGE_UP:
            planeHdg += 1.0;
            navMap->setMarker(planeLon, planeLat, planeHdg);
            break;
        case GLFW_KEY_PAGE_DOWN:
            planeHdg -= 1.0;
            navMap->setMarker(planeLon, planeLat, planeHdg);
            break;
        default:
            break;
        }
    }

    setWindowTitle(window);
}

static void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1024, 1024, "", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    setWindowTitle(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "[GLEW] ERROR: initialization failure" << std::endl;
        getchar();
        glfwTerminate();
        return EXIT_FAILURE;
    }

    int pxl_width, pxl_height;
    glfwGetFramebufferSize(window, &pxl_width, &pxl_height);
    glViewport(0, 0, pxl_width, pxl_height);

#if defined(TEST_NAVMAP_THREAD)
    NavMapThread* navMapThread = new DsfMapSimpleMarkerNavMapThread;
    navMap = navMapThread;
#else
    SimpleMarker marker;
    DsfMap dsfMap;
    dsfMap.setDsfDirectory(dsfRootDir);
    dsfMap.setTmpDirectory(tmpDir);
    navMap = new NavMap(&dsfMap, &marker);
#endif

    navMap->setMap(mapLon, mapLat);
    navMap->setMapScale(mapScaleX, mapScaleY);

    navMap->setMarker(planeLon, planeLat, planeHdg);
    navMap->setMarkerScale(0.05, 0.05);

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

#if defined(TEST_NAVMAP_THREAD)
    navMapThread->start(pxl_width, pxl_height);
#endif

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        navMap->render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

#if defined(TEST_NAVMAP_THREAD)
    navMapThread->stop();
#endif

    delete navMap;

    glfwTerminate();
    return EXIT_SUCCESS;
}