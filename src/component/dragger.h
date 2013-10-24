#ifndef __component_dragger__
#define __component_dragger__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "component.h"
#include "context.h"
#include "intersect.h"
#include "component/line.h"
#include "component/meshcomponent.h"

typedef enum {
  DRAGGER_IDLE = 1,
  DRAGGER_HIGHLIGHT,
  DRAGGER_SELECTED,
  DRAGGER_LOWLIGHT,
  DRAGGER_HIDE,
  DRAGGER_SHOW_SECOND
} DraggerState;

typedef enum {
  DRAGGER_TRANSLATE = 1,
  DRAGGER_SCALE,
  DRAGGER_ROTATE,
} DraggerType;

typedef struct _Dragger Dragger;

struct _Dragger
{
  Context* context;
  AABox box;
  double scale;
  //Line* line;
  Vec4 color_idle;
  MeshComponent* mc;
  MeshComponent* mc_second;
  DraggerState state;
  Vec3 constraint;
  DraggerType type;
  Mesh* collider;
  Quat ori;
};

ComponentDesc* dragger_desc();

void dragger_highlight_set(Dragger* d, bool highlight);
void dragger_state_set(Dragger* d, DraggerState state);

typedef Object* (*dragger_create_fn)(Vec3 constraint, Vec4 color, bool plane);
Object* _dragger_rotate_create(Vec3 constraint, Vec4 color, bool plane);
Object* _dragger_scale_create(Vec3 constraint, Vec4 color, bool plane);
Object* _dragger_translate_create(Vec3 constraint, Vec4 color, bool plane);


#endif
