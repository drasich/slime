#include "camera.h"
#include "property.h"
#include "component.h"
#include "object.h"

static void
_camera_display(Camera* c)
{
  line_clear(c->line);
  float near = c->near;
  float far = c->far;
  float hh = tan(c->fovy/2)*near;
  float hw = hh * c->aspect;
  Vec4 color = vec4(1,1,1,1);
  line_add_color(
        c->line,
        vec3(0,0,0),
        vec3(hw,hh,-near),
        color);


  line_add_color(
        c->line,
        vec3(0,0,0),
        vec3(-hw,hh,-near),
        color);

  line_add_color(
        c->line,
        vec3(0,0,0),
        vec3(-hw,-hh,-near),
        color);

  line_add_color(
        c->line,
        vec3(0,0,0),
        vec3(hw,-hh,-near),
        color);

  //rect
  line_add_color(
        c->line,
        vec3(-hw,hh,-near),
        vec3(hw,hh,-near),
        color);

  line_add_color(
        c->line,
        vec3(-hw,-hh,-near),
        vec3(hw,-hh,-near),
        color);

  line_add_color(
        c->line,
        vec3(hw,-hh,-near),
        vec3(hw,hh,-near),
        color);

  line_add_color(
        c->line,
        vec3(-hw,-hh,-near),
        vec3(-hw,hh,-near),
        color);

  return;

  float hwf = hw* far/near;
  float hhf = hh* far/near;

  //lines between near and far
  line_add_color(
        c->line,
        vec3(hw,hh,-near),
        vec3(hwf,hhf,-far),
        color);


  line_add_color(
        c->line,
        vec3(-hw,hh,-near),
        vec3(-hwf,hhf,-far),
        color);

  line_add_color(
        c->line,
        vec3(-hw,-hh,-near),
        vec3(-hwf,-hhf,-far),
        color);

  line_add_color(
        c->line,
        vec3(hw,-hh,-near),
        vec3(hwf,-hhf,-far),
        color);

  // far rect
  line_add_color(
        c->line,
        vec3(-hwf,hhf,-far),
        vec3(hwf,hhf,-far),
        color);

  line_add_color(
        c->line,
        vec3(-hwf,-hhf,-far),
        vec3(hwf,-hhf,-far),
        color);

  line_add_color(
        c->line,
        vec3(hwf,-hhf,-far),
        vec3(hwf,hhf,-far),
        color);

  line_add_color(
        c->line,
        vec3(-hwf,-hhf,-far),
        vec3(-hwf,hhf,-far),
        color);

}

static void *
_create_camera()
{
  Camera* c = calloc(1, sizeof *c);

  c->fovy = M_PI/4.0;
  c->fovy_base = M_PI/4.0;
  c->height_base = 400;
  c->near = 1.0f;
  c->far = 1000.0f;
  c->width = 800;
  c->height = c->height_base;

  c->line = create_line();
  ccamera_update_projection(c);
  _camera_display(c);

  return c;
}

static void
_camera_init(Component* component)
{
  Camera* c = component->data;
  c->fovy = c->fovy_base;
  c->height_base = c->height;
  ccamera_update_projection(c);
}

static PropertySet* 
_camera_properties()
{
  PropertySet* ps = create_property_set();

  ADD_PROP(ps, Camera, width, EET_T_DOUBLE);
  ADD_PROP(ps, Camera, height, EET_T_DOUBLE);
  ADD_PROP(ps, Camera, fovy_base, EET_T_DOUBLE);
  ADD_PROP(ps, Camera, near, EET_T_DOUBLE);
  ADD_PROP(ps, Camera, far, EET_T_DOUBLE);

  return ps;
}

static void
_camera_on_property_changed(Component* c)
{
  ccamera_update_projection(c->data);
  _camera_display(c->data);
}

static void
_camera_draw_edit(Component* comp, Matrix4 world, Matrix4 projection)
{
  Camera* c = comp->data;
  Line* l = c->line;
  line_prepare_draw(l, world, projection);
  line_draw(l);
}

ComponentDesc camera_desc = {
  "camera",
  _create_camera,
  _camera_properties,
  _camera_init,
  NULL,
  NULL,
  _camera_on_property_changed,
  _camera_draw_edit
};


void
ccamera_update_projection(Camera* c)
{
  c->aspect = (float)c->width/(float)c->height;
  c->fovy = c->fovy_base * (float)c->height/(float)c->height_base;
  mat4_set_perspective(c->projection, c->fovy, c->aspect , c->near, c->far);
}

void
ccamera_update_orthographic(Camera* c)
{
 // mat4_set_orthographic(c->orthographic, c->width/2, c->height/2, c->near, c->far);
  mat4_set_orthographic(c->orthographic, c->width/2, c->height/2, -1000, c->far);
}

void 
ccamera_set_resolution(Camera* c, int w, int h)
{
  if (w != c->width || h != c->height) {
    c->width = w;
    c->height = h;
    ccamera_update_projection(c);
    ccamera_update_orthographic(c);
  }
}

