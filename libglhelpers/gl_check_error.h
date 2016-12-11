#ifndef GL_CHECK_ERROR_H_
#define GL_CHECK_ERROR_H_

#include <GL/glew.h>
#include <iostream>

#if defined(WITH_DEBUG)
inline
static void checkGLErrors(const char* atFile, const char* atFunc, int atLine)
{
    for (GLenum err; (err = glGetError()) != GL_NO_ERROR; )
    {
        std::cerr
            << "OpenGL Error: " << err << " 0x" << std::hex << err << std::dec
            << " at " << atFile << " " << atFunc << ":" << atLine << std::endl;
    }
}
#define GL_CHECK_ERRORS    checkGLErrors(__FILE__, __func__, __LINE__)
#else
#define GL_CHECK_ERRORS
#endif

#endif
