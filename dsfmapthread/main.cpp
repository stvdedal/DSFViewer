#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <stdlib.h>

#include "dsf_map_thread.h"

static const char* vertexShaderSource = R"(
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

static const char* fragmentShaderSource = R"(
#version 330 core
in vec2 texCoord;
out vec4 color;
uniform sampler2D texSampler;
void main()
{
    color = texture(texSampler, texCoord);
}
)";

static GLfloat centr_point_lon = 46.0;
static GLfloat centr_point_lat = 51.0;
static GLfloat scale_map_x = 1.0;
static GLfloat scale_map_y = 1.0;

MapTextureProvider mapProvider;
FutureTexture tex;

GLuint makeShader(GLenum shaderType, const char* source)
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

GLuint makeProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
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

static void set_window_title(GLFWwindow* window)
{
    char buf[512];

    snprintf(buf, sizeof(buf), "DSF Map. lon=%f lat=%f sc_x=%f sc_y=%f",
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
    std::cerr << "glfw_key_callback BEGIN" << std::endl;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (action == GLFW_REPEAT || action == GLFW_PRESS)
    {
        const GLfloat coord_step = 0.05f;
        const GLfloat scale_step = 0.05f;

        switch (key)
        {
        case GLFW_KEY_LEFT:
            centr_point_lon -= coord_step;
            mapProvider.get(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y, &tex);
            break;
        case GLFW_KEY_RIGHT:
            centr_point_lon += coord_step;
            mapProvider.get(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y, &tex);
            break;
        case GLFW_KEY_UP:
            centr_point_lat += coord_step;
            mapProvider.get(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y, &tex);
            break;
        case GLFW_KEY_DOWN:
            centr_point_lat -= coord_step;
            mapProvider.get(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y, &tex);
            break;
        case GLFW_KEY_PAGE_UP:
            scale_map_x += scale_step;
            scale_map_y += scale_step;
            mapProvider.get(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y, &tex);
            break;
        case GLFW_KEY_PAGE_DOWN:
            scale_map_x -= scale_step;
            scale_map_y -= scale_step;
            if (scale_map_x < scale_step)
                scale_map_x = scale_step;
            if (scale_map_y < scale_step)
                scale_map_y = scale_step;
            mapProvider.get(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y, &tex);
            break;
        default:
            break;
        }
    }

    set_window_title(window);
    std::cerr << "glfw_key_callback END" << std::endl;
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
    set_window_title(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "[GLEW] ERROR: initialization failure" << std::endl;
        getchar();
        glfwTerminate();
        return EXIT_FAILURE;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    mapProvider.start(width, height);
    mapProvider.get(centr_point_lon, centr_point_lat, scale_map_x, scale_map_y, &tex);

    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);

    static const GLfloat vertexData[] =
    {
        // position      texture
        -1.0F, -1.0F,   0.0F, 0.0F,
        +1.0F, -1.0F,   1.0F, 0.0F,
        +1.0F, +1.0F,   1.0F, 1.0F,
        -1.0F, +1.0F,   0.0F, 1.0F
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint program = makeProgram(vertexShaderSource, fragmentShaderSource);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        tex.load();

        glBindVertexArray(VAO);
        glUseProgram(program);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
        //glfwWaitEvents();
        glfwPollEvents();
    }

    glDeleteProgram(program);
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    mapProvider.stop();
    glfwTerminate();
    return EXIT_SUCCESS;
}

