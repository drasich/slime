#ifndef __glview__
#define __glview__
#include <Elementary.h>
#include "gl.h"

void _init_gl(Evas_Object *obj);
void _del_gl(Evas_Object *obj);
void _resize_gl(Evas_Object *obj);
void _draw_gl(Evas_Object *obj);
Eina_Bool _anim(void *data);
void _del(void *data, Evas *evas, Evas_Object *obj, void *event_info);

Evas_Object* _create_glview(Evas_Object* win);
#endif
