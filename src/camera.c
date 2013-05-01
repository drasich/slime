#include "camera.h"

Camera*
create_camera()
{
  Camera* c = calloc(1,sizeof *c);
  //c->fovy = 70.0 /180.0 *M_PI;
  //c->fovy_base = 70.0 /180.0 *M_PI;
  c->fovy = M_PI/4.0;
  c->fovy_base = M_PI/4.0;
  c->height_base = 400;
  c->near = 1.0f;
  c->far = 1000.0f;
  return c;
}

void
camera_update_projection(Camera* c)
{
  c->aspect = (float)c->width/(float)c->height;
  c->fovy = c->fovy_base * (float)c->height/(float)c->height_base;
  mat4_set_perspective(c->projection, c->fovy, c->aspect , c->near, c->far);
}

void
camera_update_orthographic(Camera* c)
{
 // mat4_set_orthographic(c->orthographic, c->width/2, c->height/2, c->near, c->far);
  mat4_set_orthographic(c->orthographic, c->width/2, c->height/2, -1000, c->far);
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
 
  //Quat qy = quat_angle_axis(c->yaw, vec3(0,1,0));
  //Quat qp = quat_angle_axis(c->pitch, vec3(1,0,0));
  
  //TODO angles
  o->angles.X = c->pitch/M_PI*180.0;
  o->angles.Y = c->yaw/M_PI*180.0;
  o->Orientation = quat_angles_rad(c->pitch, c->yaw,0);
  // can also use this 
  o->Orientation = quat_angles_deg(o->angles.Y, o->angles.X, o->angles.Z);
  //o->Orientation = quat_mul(qy, qp);

  c->center = at;
  camera_recalculate_origin(c);
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

Ray
ray_from_screen(Camera* c, double x, double y, float length)
{
  double near = c->near;
  Vec3 camz = quat_rotate_vec3(c->object.Orientation, vec3(0,0,-1));
  Vec3 up = quat_rotate_vec3(c->object.Orientation, vec3(0,1,0));
  Vec3 h = vec3_cross(camz, up);
  h = vec3_normalized(h);
  double l = vec3_length(h);
  double vl = tan(c->fovy/2.0) * near;

  // used to do this : elm_glview_size_get(o, &width, &height);
  int width = c->width, height = c->height;
  double aspect = (double)width/ (double)height;
  double vh = vl * aspect;

  up = vec3_mul(up, vl);
  h = vec3_mul(h, vh);

  x -= (double)width / 2.0;
  y -= (double)height / 2.0;

  y /= (double)height / 2.0;
  x /= (double)width / 2.0;

  Vec3 pos = vec3_add(
        c->object.Position, 
        vec3_add(
          vec3_mul(camz,near),
          vec3_add(vec3_mul(h,x), vec3_mul(up,-y))
          )
        );

  Vec3 dir = vec3_sub(pos, c->object.Position);
  dir = vec3_normalized(dir);
  dir = vec3_mul(dir, length);

  Ray r = {pos, dir};
  return r;
}

void
camera_recalculate_origin(Camera* cam)
{
  Object* c = (Object*) cam;

  Vec3 offset = quat_rotate_vec3(c->Orientation, cam->local_offset);
  Vec3 origin = vec3_sub(c->Position, offset);
  cam->origin = quat_rotate_around(quat_inverse(c->Orientation), cam->center, origin);
}
