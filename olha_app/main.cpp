#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <stdlib.h>

#include "navmap.h"

static double planePosLon = 46.0;
static double planePosLat = 51.2;
static double planePosHdg = 45.0;
static double mapScaleX = 1.0;
static double mapScaleY = 1.0;

NavMap* navMap;

static void setWindowTitle(GLFWwindow* window)
{
    char buf[512];
    snprintf(buf, sizeof(buf), "Nav Map. lon=%f lat=%f hdg=%f sc_x=%f sc_y=%f",
        planePosLon, planePosLat, planePosHdg, mapScaleX, mapScaleY
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
                planePosLon -= coordStep;
            else if (mods & GLFW_MOD_SHIFT)
                planePosHdg -= 1.0;
            navMap->setPlane(planePosLon, planePosLat, planePosHdg);
            break;
        case GLFW_KEY_RIGHT:
            if (mods == 0)
                planePosLon += coordStep;
            else if (mods & GLFW_MOD_SHIFT)
                planePosHdg += 1.0;
            navMap->setPlane(planePosLon, planePosLat, planePosHdg);
            break;
        case GLFW_KEY_UP:
            planePosLat += coordStep;
            navMap->setPlane(planePosLon, planePosLat, planePosHdg);
            break;
        case GLFW_KEY_DOWN:
            planePosLat -= coordStep;
            navMap->setPlane(planePosLon, planePosLat, planePosHdg);
            break;
        case GLFW_KEY_PAGE_UP:
            mapScaleX += scaleStep;
            mapScaleY += scaleStep;
            navMap->setScale(mapScaleX, mapScaleY);
            break;
        case GLFW_KEY_PAGE_DOWN:
            mapScaleX -= scaleStep;
            mapScaleY -= scaleStep;
            if (mapScaleX < scaleStep)
                mapScaleX = scaleStep;
            if (mapScaleY < scaleStep)
                mapScaleY = scaleStep;
            navMap->setScale(mapScaleX, mapScaleY);
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

    navMap = new NavMap;
    navMap->setTextureSize(int(pxl_width), int(pxl_height));
    navMap->setPlaneScale(0.05, 0.05);
    navMap->setPlane(planePosLon, planePosLat, planePosHdg);
    navMap->setScale(mapScaleX, mapScaleY);

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        navMap->render();
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}
