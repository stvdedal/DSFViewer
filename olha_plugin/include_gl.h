#ifndef OLHA_INCLUDE_GL_H_
#define OLHA_INCLUDE_GL_H_

#if IBM
#include <windows.h>
#endif
#if LIN
#include <GL/gl.h>
#else
#if __GNUC__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif

#endif
