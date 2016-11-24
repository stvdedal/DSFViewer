#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <iomanip>
#include <map>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsf_render.h"
#include "dsf_file.h"

dsf::File dsfFile;
DsfRender dsfRender;

static const char* vertex_shader_str =
"#version 330 core"                                                 "\n"
"layout(location = 0) in vec3 position;"                            "\n"
"void main()"                                                       "\n"
"{"                                                                 "\n"
"    gl_Position = vec4(position.x, position.y, position.z, 1.0);"  "\n"
"}"                                                                 "\n";

static const char* fragment_shader_str =
"#version 330 core"                                                 "\n"
"out vec4 color;"                                                   "\n"
"void main()"                                                       "\n"
"{"                                                                 "\n"
"    color = vec4(1.0f, 0.5f, 0.2f, 1.0f);"                         "\n"
"}"                                                                 "\n";



GLuint VAO, VBO;
GLuint shaderProgram;

static void setup(GLFWwindow* window)
{
    GLint success;
    GLchar infoLog[512];

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_str, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_str, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //////////////////////////////////

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, // Left
        0.5f, -0.5f, 0.0f, // Right
        0.0f,  0.5f, 0.0f  // Top
    };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
}

static void render(GLFWwindow* window)
{
    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw our first triangle
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d. %s\n", error, description);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
    int pxl_width, pxl_height;
    glfwGetFramebufferSize(window, &pxl_width, &pxl_height);
    glViewport(0, 0, pxl_width, pxl_height);

    render(window);
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage:\ndfsviewer <file.dsf>" << std::endl;
        getchar();
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    if (!dsfFile.open(filename.c_str()))
    {
        std::cerr << "ERROR: failed to open " << filename << std::endl;
        getchar();
        return EXIT_FAILURE;
    }

    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, ("DSF Viewer: " + filename).c_str(), nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    int pxl_width, pxl_height;
    glfwGetFramebufferSize(window, &pxl_width, &pxl_height);
    glViewport(0, 0, pxl_width, pxl_height);

    setup(window);

    int c = 0;
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

