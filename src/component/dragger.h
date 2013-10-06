#ifndef __component_dragger__
#define __component_dragger__
#include "vec.h"
#include "quat.h"
#include "matrix.h"
#include "component.h"
#include "context.h"
#include "intersect.h"
#include "component/line.h"

typedef struct _Dragger Dragger;

struct _Dragger
{
  Context* context;
  AABox box;
  double scale;
  Line* line;
  Vec4 color;
  Vec4 color_wanted;
};

ComponentDesc* dragger_desc();

void dragger_highlight_set(Dragger* d, bool highlight);

#endif
