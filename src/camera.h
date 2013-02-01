#ifndef __camera__
#define __camera__

typedef struct _Camera Camera;
struct _Camera
{
  Vec3 position;
  Vec3 rotation;

  //other things like fov etc..

}

void camera_pan(Camera* cam, Vec3 v); // same as trans
void camera_rotate(Camera* cam, Quat q); // same as -pos, rot, + pos
void camera_rotate_around_position(Camera* cam, Quat q, Vec3 v);  //same as rotate the cam pos and look at v

#endif
