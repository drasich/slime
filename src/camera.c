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

  //double yaw = atan2(d.X,d.Z);
  //double pitch = atan2(d.Y,d.Z);

  //c->yaw = atan2(-d.X,-d.Z);
  c->yaw = atan2(d.X,-d.Z);
  c->pitch = atan2(-d.Y,-d.Z);

  printf("yaw : %f\n", c->yaw);
  printf("pitch : %f\n", c->pitch);
  printf("yaw degree : %f\n", c->yaw* 180/M_PI);
  printf("pitch degree : %f\n", c->pitch*180/M_PI);

  Quat qy = quat_angle_axis(c->yaw, vec3(0,1,0));
  Quat qp = quat_angle_axis(c->pitch, vec3(1,0,0));
  
  o->Orientation = quat_mul(qy, qp);
  c->forward = vec3_sub(at, o->Position);
  c->forward = vec3_normalized(c->forward);


}
