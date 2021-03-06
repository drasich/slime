#include "camera.h"

ViewCamera*
view_camera_new()
{
  ViewCamera* c = calloc(1,sizeof *c);
  c->object = create_object();
  c->object->name = "viewcamera";
  Component* comp = create_component(component_camera_desc());
  object_add_component(c->object,comp);
  c->camera_component = comp->data;
  return c;
}

void 
camera_lookat(ViewCamera* cam, Vec3 at)
{
  Object* o = cam->object;
  Camera* c = cam->camera_component;

  Vec3 d = vec3_sub(at, o->position);

  c->yaw = atan2(-d.x,-d.z);
  float r = sqrt( d.x*d.x + d.z*d.z );
  c->pitch = atan2(d.y,r);

  o->angles.x = c->pitch/M_PI*180.0;
  o->angles.y = c->yaw/M_PI*180.0;
  o->orientation = quat_yaw_pitch_roll_deg(o->angles.y, o->angles.x, o->angles.z);

  c->center = at;
  camera_recalculate_origin(cam);
}

void
camera_rotate_around(ViewCamera* cam, Quat q, Vec3 pivot)
{
  Object* o = cam->object;
  Camera* c = cam->camera_component;

  Vec3 def = quat_rotate_around(q, pivot, c->origin);
  Vec3 doff = quat_rotate_vec3(q, c->local_offset);
  o->position = vec3_add(def, doff);
}

void
camera_pan(ViewCamera* cam, Vec3 t)
{
  Object* o = cam->object;
  Camera* c = cam->camera_component;

  c->local_offset = vec3_add(c->local_offset, t);
  t = quat_rotate_vec3(o->orientation, t);
  o->position = vec3_add(o->position, t);
}

Ray
ray_from_screen(ViewCamera* cam, double x, double y, float length)
{
  Object* o = cam->object;
  Camera* c = cam->camera_component;

  double near = c->near;
  Vec3 camz = quat_rotate_vec3(o->orientation, vec3(0,0,-1));
  Vec3 up = quat_rotate_vec3(o->orientation, vec3(0,1,0));
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
        o->position, 
        vec3_add(
          vec3_mul(camz,near),
          vec3_add(vec3_mul(h,x), vec3_mul(up,-y))
          )
        );

  Vec3 dir = vec3_sub(pos, o->position);
  dir = vec3_normalized(dir);
  dir = vec3_mul(dir, length);

  Ray r = {pos, dir};
  return r;
}

void
camera_recalculate_origin(ViewCamera* cam)
{
  Object* o = cam->object;
  Camera* c = cam->camera_component;

  Vec3 offset = quat_rotate_vec3(o->orientation, c->local_offset);
  Vec3 origin = vec3_sub(o->position, offset);
  c->origin = quat_rotate_around(quat_inverse(o->orientation), c->center, origin);
}

void
camera_get_frustum(ViewCamera* cam, Frustum* out)
{
  Object* o = cam->object;
  Camera* c = cam->camera_component;

  out->near = c->near;
  out->far = c->far;
  out->direction = quat_rotate_vec3(o->orientation, vec3(0,0,-1));
  out->start = o->position;
  out->up = quat_rotate_vec3(o->orientation, vec3(0,1,0));
  out->fovy = c->fovy;
  out->aspect = c->aspect;
}

void
camera_get_frustum_planes(ViewCamera* cam, Plane* p)
{
  Object* o = cam->object;
  Camera* c = cam->camera_component;

  //near plane
  Vec3 direction = quat_rotate_vec3(o->orientation, vec3(0,0,-1));
  Vec3 right = quat_rotate_vec3(o->orientation, vec3(1,0,0));
  Vec3 up = quat_rotate_vec3(o->orientation, vec3(0,1,0));

  p[0].point = vec3_add(o->position, vec3_mul(direction, c->near));
  p[0].normal = direction;

  //far plane
  p[1].point = vec3_add(o->position, vec3_mul(direction, c->far));
  p[1].normal = vec3_mul(direction, -1);

  //up plane
  float hh = tan(c->fovy/2)* c->near;
  Vec3 upd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(up, hh));

  p[2].point = o->position;
  Vec3 nn = vec3_normalized(vec3_cross(right, upd));
  p[2].normal = vec3_mul(nn, -1);

  /*
  printf(" upd : %f, %f, %f \n", upd.x, upd.y, upd.z);
  printf(" up : %f, %f, %f \n", up.x, up.y, up.z);
  printf(" up plane normal : %f, %f, %f \n", nn.x, nn.y, nn.z);
  */

  //down plane
  p[3].point = o->position;
  Vec3 downd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(up, -hh));
  nn = vec3_normalized(vec3_cross(right, downd));
  //p[3].normal = vec3_mul(nn, -1);
  p[3].normal = nn;


  //right plane
  float hw = hh * c->aspect;
  p[4].point = o->position;
  Vec3 rightd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(right, hw));
  nn = vec3_normalized(vec3_cross(up, rightd));
  //p[4].normal = vec3_mul(nn, -1);
  p[4].normal = nn;

  //left plane
  p[5].point = o->position;
  Vec3 leftd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(right, -hw));
  nn = vec3_normalized(vec3_cross(up, leftd));
  p[5].normal = vec3_mul(nn, -1);

}

void
camera_get_frustum_planes_rect(
      ViewCamera* cam,
      Plane* p,
      float left, float top, float width, float height)
{

  Object* o = cam->object;
  Camera* c = cam->camera_component;

  Vec3 direction = quat_rotate_vec3(o->orientation, vec3(0,0,-1));
  Vec3 right = quat_rotate_vec3(o->orientation, vec3(1,0,0));
  Vec3 up = quat_rotate_vec3(o->orientation, vec3(0,1,0));

  //plane order:
  //near, far, up, down, right, left

  //near plane
  p[0].point = vec3_add(o->position, vec3_mul(direction, c->near));
  p[0].normal = direction;

  //far plane
  p[1].point = vec3_add(o->position, vec3_mul(direction, c->far));
  p[1].normal = vec3_mul(direction, -1);

  //up plane
  float hh = tan(c->fovy/2)* c->near;
  top = top * hh / (c->height/2.0f);
  height = height * hh / (c->height/2.0f);

  float th = hh - top;
  Vec3 upd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(up, th));

  p[2].point = o->position;
  Vec3 nn = vec3_normalized(vec3_cross(right, upd));
  p[2].normal = vec3_mul(nn, -1);

  //down plane
  float bh = hh - (top + height);
  p[3].point = o->position;
  Vec3 downd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(up, bh));
  nn = vec3_normalized(vec3_cross(right, downd));
  //p[3].normal = vec3_mul(nn, -1);
  p[3].normal = nn;


  //right plane
  float hw = hh * c->aspect;
  left = left * hw / (c->width/2.0f);
  width = width * hw / (c->width/2.0f);

  float rw = -hw + (left + width);
  p[4].point = o->position;
  Vec3 rightd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(right, rw));
  nn = vec3_normalized(vec3_cross(up, rightd));
  //p[4].normal = vec3_mul(nn, -1);
  p[4].normal = nn;

  //left plane
  float lw = -hw + left;
  p[5].point = o->position;
  Vec3 leftd = vec3_add(
        vec3_mul(direction, c->near),
        vec3_mul(right, lw));
  nn = vec3_normalized(vec3_cross(up, leftd));
  p[5].normal = vec3_mul(nn, -1);

  /*
  printf(" leftd : %f, %f, %f \n", leftd.x, leftd.y, leftd.z);
  printf(" up : %f, %f, %f \n", up.x, up.y, up.z);
  printf(" up plane normal : %f, %f, %f \n", nn.x, nn.y, nn.z);
  */
}

Vec2
camera_world_to_screen(ViewCamera* vc, Vec3 p)
{
  Matrix4 cam_inv;
  mat4_inverse(vc->object->matrix, cam_inv);
  Matrix4* projection = &vc->camera_component->projection;

  Matrix4 tm;
  mat4_multiply(*projection, cam_inv, tm);

  Vec4 p4 = vec4(p.x, p.y, p.z, 1.0);
  Vec4 sp = mat4_vec4_mul(tm, p4);

  Vec3 n = vec3(sp.x/sp.w,sp.y/sp.w, sp.z/sp.w);

  Vec2 screen =  vec2(
        (n.x+1.0)* vc->camera_component->width/2.0,
        -(n.y-1.0)* vc->camera_component->height/2.0);

  //printf("screen : %f, %f \n", screen.x, screen.y);

  return screen;
}
