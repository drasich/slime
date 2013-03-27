#ifndef __ui_property__
#define __ui_property__

Evas_Object* property_create(Evas_Object* win);
Evas_Object* property_add(
      Evas_Object* win, 
      Evas_Object* bx, 
      char* name, 
      char* value);

void property_change(char* name, char* value);
#endif
