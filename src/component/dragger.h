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
  DRAGGER_IDLE,
  DRAGGER_HIGHLIGHT,
  DRAGGER_SELECTED
} DraggerState;

typedef struct _Dragger Dragger;

struct _Dragger
{
  Context* context;
  AABox box;
  double scale;
  Line* line;
  //Vec4 color;
  MeshComponent* mc;
  DraggerState state;
  Vec3 constraint;
};

ComponentDesc* dragger_desc();

void dragger_highlight_set(Dragger* d, bool highlight);
void dragger_state_set(Dragger* d, DraggerState state);

#endif
