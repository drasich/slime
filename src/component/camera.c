#include "camera.h"
#include "property.h"
#include "component.h"
#include "object.h"

static void *
_create_camera()
{
  CCamera* c = calloc(1, sizeof *c);

  c->fovy = M_PI/4.0;
  c->fovy_base = M_PI/4.0;
  c->height_base = 400;
  c->near = 1.0f;
  c->far = 1000.0f;

  c->line = ccreate_line();
  AABox aabox = { vec3(-1,-1,-1), vec3(1,1,1)};
  cline_add_box(c->line,aabox, vec4(1,1,1,1));
  return c;
}

static Eina_Inarray* 
_camera_properties()
{
  Eina_Inarray * iarr = create_property_set();

  ADD_PROP(iarr, CCamera, fovy_base, EET_T_DOUBLE);
  ADD_PROP(iarr, CCamera, near, EET_T_DOUBLE);
  ADD_PROP(iarr, CCamera, far, EET_T_DOUBLE);

  return iarr;
}

static void
_camera_on_property_changed(Component* c)
{
  ccamera_update_projection(c->data);
}

static void
_camera_draw_edit(Component* c)
{
  /*
  //draw line
  CCamera* cam = c->data;
  Object* o = c->object;
  Matrix4 cam_mat_inv, mo;

  object_compute_matrix(o, mo);
  mat4_multiply(cam_mat_inv, mo, mo);
  object_draw_lines_camera(o, mo, cam);

  line_draw(cam->line);
  */

}

ComponentDesc camera_desc = {
  "camera",
  _create_camera,
  _camera_properties,
  NULL,
  NULL,
  NULL,
  _camera_on_property_changed,
  _camera_draw_edit
};


void
ccamera_update_projection(CCamera* c)
{
  c->aspect = (float)c->width/(float)c->height;
  c->fovy = c->fovy_base * (float)c->height/(float)c->height_base;
  mat4_set_perspective(c->projection, c->fovy, c->aspect , c->near, c->far);
}

void
ccamera_update_orthographic(CCamera* c)
{
 // mat4_set_orthographic(c->orthographic, c->width/2, c->height/2, c->near, c->far);
  mat4_set_orthographic(c->orthographic, c->width/2, c->height/2, -1000, c->far);
}

void 
ccamera_set_resolution(CCamera* c, int w, int h)
{
  if (w != c->width || h != c->height) {
    c->width = w;
    c->height = h;
    ccamera_update_projection(c);
    ccamera_update_orthographic(c);
  }
}

