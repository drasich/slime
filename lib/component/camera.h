#ifndef __component_camera__
#define __component_camera__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "component.h"
//#include "component/line.h"
#include "component/linecomponent.h"

#define ENUM(name, ... ) \
 typedef enum { __VA_ARGS__ } name; \
 static const char* name ## _enum_string = # __VA_ARGS__;

typedef struct _Camera Camera;
//typedef enum {PERSPECTIVE, ORTHOGRAPHIC} Projection;
ENUM(Projection, PERSPECTIVE, ORTHOGRAPHIC);

#define ENUM_STRINGS_GET(name, num) \
 eina_str_split_full( name ## _enum_string, ",", 0, num);

struct _Camera
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
  LineComponent* line;
};

//ComponentDesc camera_desc;
ComponentDesc* component_camera_desc();

void ccamera_recalculate_origin(Component* comp);
void ccamera_set_resolution(Camera* c, int w, int h);
void ccamera_update_projection(Camera* c);
void camera_init(Camera* c);

#endif
