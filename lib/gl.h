#ifndef __gl__
#define __gl__

#if defined EVAS_GL

#include "Evas_GL.h"
Evas_GL_API* gl;

#else

#include "GLES2/gl2.h"
//#define gl()

#endif

#endif

