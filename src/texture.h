#ifndef __texture__
#define __texture__

#include "gl.h"

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
//void save_png(GLfloat* pix);
void save_png(GLuint* pix);

#endif
