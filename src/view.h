#ifndef _VIEW_H__
#define _VIEW_H__
#include "matrix.h"
#include "camera.h"
#include "fbo.h"
#include "ui/property.h"
#include "ui/tree.h"

typedef struct _Render Render;
struct _Render
{
  Object* quad_outline;
  Object* quad_color;
  Fbo* fbo_selected;
  Fbo* fbo_all;
};


typedef struct _View View;

struct _View
{
  struct _Context* context;
  struct _Control* control;
  Evas_Object* glview;
  Property* property;
  Tree* tree;
  Evas_Object* box;
  Camera* camera;
  Render* render;
  Object* repere;
  Object* grid;
  Object* camera_repere;

  Evas_Object* select_rect;
};

View* create_view(Evas_Object *win);
void view_destroy(View* v);
void view_update(View* v, double dt);

Render* create_render();
void view_draw(View* v);

#endif
