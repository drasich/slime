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

void
camera_get_frustum(Camera* c, Frustum* out)
{
  out->near = c->near;
  out->far = c->far;
  out->direction = quat_rotate_vec3(c->object.Orientation, vec3(0,0,-1));
  out->start = c->object.Position;
  out->up = quat_rotate_vec3(c->object.Orientation, vec3(0,1,0));
  out->fovy = c->fovy;
  out->aspect = c->aspect;
}

void
camera_get_frustum_planes(Camera* c, Plane* p)
{
  //near plane
  Vec3 direction = quat_rotate_vec3(c->object.Orientation, vec3(0,0,-1));
  Vec3 right = quat_rotate_vec3(c->object.Orientation, vec3(1,0,0));
  Vec3 up = quat_rotate_vec3(c->object.Orientation, vec3(0,1,0));

  p[0].Point = vec3_add(c->object.Position, vec3_mul(direction, c->near));
  p[0].Normal = direction;

  //far plane
  p[1].Point = vec3_add(c->object.Position, vec3_mul(direction, c->far));
  p[1].Normal = vec3_mul(direction, -1);

  //up plane
  float hh = tan(c->fovy/2)* c->near;
  Vec3 upd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(up, hh));

  p[2].Point = c->object.Position;
  Vec3 nn = vec3_normalized(vec3_cross(right, upd));
  p[2].Normal = vec3_mul(nn, -1);

  /*
  printf(" upd : %f, %f, %f \n", upd.X, upd.Y, upd.Z);
  printf(" up : %f, %f, %f \n", up.X, up.Y, up.Z);
  printf(" up plane normal : %f, %f, %f \n", nn.X, nn.Y, nn.Z);
  */

  //down plane
  p[3].Point = c->object.Position;
  Vec3 downd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(up, -hh));
  nn = vec3_normalized(vec3_cross(right, downd));
  //p[3].Normal = vec3_mul(nn, -1);
  p[3].Normal = nn;


  //right plane
  float hw = hh * c->aspect;
  p[4].Point = c->object.Position;
  Vec3 rightd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(right, hw));
  nn = vec3_normalized(vec3_cross(up, rightd));
  //p[4].Normal = vec3_mul(nn, -1);
  p[4].Normal = nn;

  //left plane
  p[5].Point = c->object.Position;
  Vec3 leftd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(right, -hw));
  nn = vec3_normalized(vec3_cross(up, leftd));
  p[5].Normal = vec3_mul(nn, -1);

}

void
camera_get_frustum_planes_rect(
      Camera* c,
      Plane* p,
      float left, float top, float width, float height)
{

  Vec3 direction = quat_rotate_vec3(c->object.Orientation, vec3(0,0,-1));
  Vec3 right = quat_rotate_vec3(c->object.Orientation, vec3(1,0,0));
  Vec3 up = quat_rotate_vec3(c->object.Orientation, vec3(0,1,0));

  //plane order:
  //near, far, up, down, right, left

  //near plane
  p[0].Point = vec3_add(c->object.Position, vec3_mul(direction, c->near));
  p[0].Normal = direction;

  //far plane
  p[1].Point = vec3_add(c->object.Position, vec3_mul(direction, c->far));
  p[1].Normal = vec3_mul(direction, -1);

  //up plane
  float hh = tan(c->fovy/2)* c->near;
  top = top * hh / (c->height/2.0f);
  height = height * hh / (c->height/2.0f);

  float th = hh - top;
  Vec3 upd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(up, th));

  p[2].Point = c->object.Position;
  Vec3 nn = vec3_normalized(vec3_cross(right, upd));
  p[2].Normal = vec3_mul(nn, -1);

  //down plane
  float bh = hh - (top + height);
  p[3].Point = c->object.Position;
  Vec3 downd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(up, bh));
  nn = vec3_normalized(vec3_cross(right, downd));
  //p[3].Normal = vec3_mul(nn, -1);
  p[3].Normal = nn;


  //right plane
  float hw = hh * c->aspect;
  left = left * hw / (c->width/2.0f);
  width = width * hw / (c->width/2.0f);

  float rw = -hw + (left + width);
  p[4].Point = c->object.Position;
  Vec3 rightd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(right, rw));
  nn = vec3_normalized(vec3_cross(up, rightd));
  //p[4].Normal = vec3_mul(nn, -1);
  p[4].Normal = nn;

  //left plane
  float lw = -hw + left;
  p[5].Point = c->object.Position;
  Vec3 leftd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(right, lw));
  nn = vec3_normalized(vec3_cross(up, leftd));
  p[5].Normal = vec3_mul(nn, -1);

  /*
  printf(" leftd : %f, %f, %f \n", leftd.X, leftd.Y, leftd.Z);
  printf(" up : %f, %f, %f \n", up.X, up.Y, up.Z);
  printf(" up plane normal : %f, %f, %f \n", nn.X, nn.Y, nn.Z);
  */
}

