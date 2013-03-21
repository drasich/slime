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

void 
camera_lookat(Camera* c, Vec3 at)
{
  Object* o = (Object*) c;
  Vec3 d = vec3_sub(at, o->Position);

  c->yaw = atan2(d.X,-d.Z);
  c->pitch = atan2(-d.Y,-d.Z);

  Quat qy = quat_angle_axis(c->yaw, vec3(0,1,0));
  Quat qp = quat_angle_axis(c->pitch, vec3(1,0,0));
  
  o->Orientation = quat_mul(qy, qp);

}

void
camera_rotate_around(Camera* c, Quat q, Vec3 pivot)
{
  Vec3 def = quat_rotate_around(q, pivot, c->origin);
  Vec3 doff = quat_rotate_vec3(q, c->local_offset);
  c->object.Position = vec3_add(def, doff);

}

void
camera_pan(Camera* c, Vec3 t)
{
  c->local_offset = vec3_add(c->local_offset, t);
  t = quat_rotate_vec3(c->object.Orientation, t);
  c->object.Position = vec3_add(c->object.Position, t);
}
