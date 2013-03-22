#ifndef _VIEW_H__
#define _VIEW_H__
#include "matrix.h"
#include "camera.h"

typedef struct _View View;

struct _View
{
  struct _Context* context;
  Evas_Object* glview;
  Evas_Object* box;
  Camera* camera;
};

View* create_view(Evas_Object *win);
void view_destroy(View* v);
void view_update(View* v, double dt);

#endif
