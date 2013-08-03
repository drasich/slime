#ifndef __component_camera__
#define __component_camera__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "component.h"
#include "component/line.h"

typedef struct _CCamera CCamera;
typedef enum {PERSPECTIVE, ORTHOGRAPHIC} Projection;

struct _CCamera
{
  Matrix4 projection;
  Matrix4 orthographic;
  Projection projection_mode;

  double fovy;
  double fovy_base;
  double near;
  double far;
  double aspect;
  double width;
  double height;
  int height_base;
  double yaw, pitch, roll;

  Vec3 origin;
  Vec3 local_offset;
  Vec3 center;

  Vec4 clear_color;
  Line* line;
};

ComponentDesc camera_desc;

void ccamera_recalculate_origin(Component* comp);
void ccamera_set_resolution(CCamera* c, int w, int h);
void ccamera_update_projection(CCamera* c);

#endif
