#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <stdlib.h>

#include <dsf_map.h>

static double centr_point_lon = 46.0;
static double centr_point_lat = 51.0;
static double scale_map_x = 1.0;
static double scale_map_y = 1.0;

DsfMap *dsfmap;

static void set_window_title(GLFWwindow* window)
{
    char buf[512];

    snprintf(buf, sizeof(buf), "DSF Map Viewer. lon=%f lat=%f sc_x=%f sc_y=%f",
        centr_point_lon, centr_point_lat, scale_map_x, scale_map_y
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
        const double coord_step = 0.05;
        const double scale_step = 0.05;

        switch (key)
        {
        case GLFW_KEY_LEFT:
            centr_point_lon -= coord_step;
            dsfmap->prepare(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y);
            break;
        case GLFW_KEY_RIGHT:
            centr_point_lon += coord_step;
            dsfmap->prepare(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y);
            break;
        case GLFW_KEY_UP:
            centr_point_lat += coord_step;
            dsfmap->prepare(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y);
            break;
        case GLFW_KEY_DOWN:
            centr_point_lat -= coord_step;
            dsfmap->prepare(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y);
            break;
        case GLFW_KEY_PAGE_UP:
            scale_map_x += scale_step;
            scale_map_y += scale_step;
            dsfmap->prepare(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y);
            break;
        case GLFW_KEY_PAGE_DOWN:
            scale_map_x -= scale_step;
            scale_map_y -= scale_step;
            if (scale_map_x < scale_step)
                scale_map_x = scale_step;
            if (scale_map_y < scale_step)
                scale_map_y = scale_step;
            dsfmap->prepare(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y);
            break;
        default:
            break;
        }
    }

    set_window_title(window);
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

    GLFWwindow* window = glfwCreateWindow(1024, 1024, "DSF Map Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    set_window_title(window);

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

    dsfmap = new DsfMap;
    dsfmap->prepare(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y);

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    while (!glfwWindowShouldClose(window))
    {
        dsfmap->render();
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

