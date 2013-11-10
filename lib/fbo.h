#ifndef __fbo__
#define __fbo__
#include "gl.h"

typedef struct _Fbo Fbo;

struct _Fbo {
  GLuint texture_depth_stencil_id;
  GLuint texture_color;
  GLuint fbo;
  GLuint rb;
};

Fbo* create_fbo();
void fbo_use(Fbo* fbo);
void fbo_use_end();
void fbo_resize(Fbo* f, int w, int h);
void fbo_destroy(Fbo* s);

#endif
