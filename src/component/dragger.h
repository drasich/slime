#ifndef __component_dragger__
#define __component_dragger__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "component.h"
#include "context.h"
#include "intersect.h"
#include "component/line.h"

typedef enum {
  DRAGGER_IDLE = 1,
  DRAGGER_HIGHLIGHT,
  DRAGGER_SELECTED,
  DRAGGER_HIDE
} DraggerState;

typedef enum {
  DRAGGER_TRANSLATE = 1,
  DRAGGER_SCALE,
  DRAGGER_ROTATE,
} DraggerType;

typedef enum {
  DRAGGER_AXE_X = 1,
  DRAGGER_AXE_Y,
  DRAGGER_AXE_Z,
  DRAGGER_PLANE_XY,
  DRAGGER_PLANE_XZ,
  DRAGGER_PLANE_YZ,
  DRAGGER_XYZ
} DraggerConstraint;



typedef struct _Dragger Dragger;

struct _Dragger
{
  Context* context;
  AABox box;
  double scale;
  //Line* line;
  Vec4 color_idle;
  MeshComponent* mc;
  DraggerState state;
  Vec3 constraint;
  DraggerType type;
  Mesh* collider;
  Quat ori;
};

ComponentDesc* dragger_desc();

void dragger_highlight_set(Dragger* d, bool highlight);
void dragger_state_set(Dragger* d, DraggerState state);

#endif
