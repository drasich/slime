#include "camera.h"

Camera*
create_camera()
{
  Camera* c = calloc(1,sizeof *c);
  c->fovy = M_PI/4.0;
  c->near = 1.0f;
  c->far = 1000.0f;
  return c;
}

void
camera_update_projection(Camera* c)
{
  c->aspect = (float)c->width/(float)c->height;
  mat4_set_perspective(c->projection, c->fovy, c->aspect , c->near, c->far);
}

void
camera_update_orthographic(Camera* c)
{
  mat4_set_orthographic(c->orthographic, c->width/2, c->height/2, c->near, c->far);
}



void 
camera_set_resolution(Camera* c, int w, int h)
{
  if (w != c->width || h != c->height) {
    c->width = w;
    c->height = h;
    camera_update_projection(c);
    camera_update_orthographic(c);
  }
}

