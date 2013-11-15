#ifndef __VIEW_H__
#define __VIEW_H__
#include "matrix.h"
#include "camera.h"
#include "fbo.h"
#include "ui/property_view.h"
//#include "ui/tree.h"


typedef struct _RenderObject RenderObject;
struct _RenderObject
{
  Matrix4 world;
  Object* object;
};

typedef struct _Render Render;
struct _Render
{
  Object* quad_outline;
  Object* quad_color;
  Fbo* fbo_selected;
  Fbo* fbo_all;

  Eina_List* render_objects;
  Eina_List* render_objects_selected;
};

typedef struct _View View;

struct _View
{
  struct _Context* context;
  struct _Control* control;
  Evas_Object* glview;
  PropertyView* property;
  struct _Tree* tree;
  Evas_Object* table;
  Evas_Object* scene_entry;
  ViewCamera* camera;
  Render* render;
  Object* repere;
  Object* grid;
  Object* camera_repere;

  Evas_Object* select_rect;
  Evas_Object* menu;

  Eina_List* draggers;
  Eina_List* dragger_translate;
  Eina_List* dragger_rotate;
  Eina_List* dragger_scale;
};

View* view_new(Evas_Object *win);
void view_destroy(View* v);
void view_update(View* v, double dt);

Render* create_render();
void view_draw(View* v);

void view_scene_set(View* v, Scene* s);

#endif
