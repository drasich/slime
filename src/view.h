#ifndef _VIEW_H__
#define _VIEW_H__
#include "matrix.h"

typedef struct _View View;

struct _View
{
  Matrix4 projection;
  struct _Context* context;
  Evas_Object* glview;
  Evas_Object* box;
};

View* create_view(Evas_Object *win);
void view_destroy(View* v);

#endif
