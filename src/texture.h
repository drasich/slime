#ifndef __texture__
#define __texture__
#include <stdbool.h>

#include "gl.h"

typedef struct _Texture Texture;

struct _Texture
{
  GLuint  width;
  GLuint  height;

  GLenum  format;
  GLint   internal_format;

  GLubyte *data;
  const char* filename;
  bool is_init;

  GLuint id;
};

Texture* texture_new();

Texture *texture_read_png_file(const char *filename);
bool texture_png_read(Texture* t);
//void save_png(GLfloat* pix);
void save_png(GLuint* pix, int width, int height);

void texture_init(Texture* t);


#endif
