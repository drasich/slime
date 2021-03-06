#ifndef __gameview_h__
#define __gameview_h__
#include "matrix.h"
//#include "camera.h"
#include "fbo.h"
#include "view.h"
#include "component.h"

//TODO make a renderer interface

typedef struct _GameView GameView;

struct _GameView
{
  Evas_Object* glview;
  Evas_Object* box;
  //Render* render;
  struct _Scene* scene;
  Evas_Object** window;
  ComponentManager* component_manager;
  Control* control;
};

GameView* create_gameview(Evas_Object *win);
Evas_Object* create_gameview_window(Scene* s, Evas_Object** window, Control* c);
//void gameview_destroy(View* v);
//void gameview_update(View* v, double dt);

//Render* create_render();
void gameview_draw(GameView* v);

#endif
