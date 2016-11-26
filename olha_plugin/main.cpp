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


#include <XPLMDisplay.h>
#include <XPLMDataAccess.h>
#include <XPLMGraphics.h>
#include <XPLMUtilities.h>

