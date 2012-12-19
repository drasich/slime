#ifndef __texture__
#define __texture__

#include <Evas_GL.h>

typedef struct _Texture Texture;

struct _Texture
{
  GLuint  width;
  GLuint  height;

  GLenum  format;
  GLint   internal_format;

  GLubyte *data;
};


Texture *texture_read_png_file(const char *filename);

#endif
