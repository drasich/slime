#include "context.h"

void
context_set_object(Context* c, struct _Object* o)
{
  c->object = o;
}
