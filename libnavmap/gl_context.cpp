#include "gl_context.h"
#include <GLFW/glfw3.h>
#include <iostream>

GlContext::GlContext()
{
    if (!glfwInit()) {
        std::cerr << "ERROR: GLFW init error" << std::endl;
        return;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1, 1, "Thread Hidden window", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "ERROR: creation window error" << std::endl;
        glfwTerminate();
        return;
    }
    glfwHideWindow(window);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return;
    }
}

GlContext::~GlContext()
{
    glfwTerminate();
}
