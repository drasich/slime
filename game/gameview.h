#ifndef __gameview_h__
#define __gameview_h__
#include "matrix.h"
//#include "camera.h"
#include "fbo.h"
#include "component.h"

//TODO make a renderer interface

typedef struct _GameView GameView;

struct _GameView
{
  Evas_Object* glview;
  Evas_Object* box;
  struct _Scene* scene;
  ComponentManager* component_manager;
};

GameView* create_gameview(Evas_Object *win);
void gameview_destroy(GameView* v);
//void gameview_update(View* v, double dt);

//Render* create_render();
void gameview_draw(GameView* v);

#endif
