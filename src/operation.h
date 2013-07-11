#ifndef __operation__
#define __operation__
#include "control.h"

///////////////////////////////////////////////
typedef struct _Op_Move_Object Op_Move_Object;
struct _Op_Move_Object
{
  Eina_List* objects;
  Vec3 translation;
};

void operation_move_object_do(Control *c, void* data);
void operation_move_object_undo(Control *c, void* data);

////////////////////////////////////////////
typedef struct _Op_Add_Object Op_Add_Object;
struct _Op_Add_Object
{
  Scene* s;
  Object* o;
};

void operation_add_object_do(Control *c, void* data);
void operation_add_object_undo(Control *c, void* data);
void control_add_object(Control* c, Scene* s, Object* o);
////////////////////////////////////////////

typedef struct _Op_Remove_Object Op_Remove_Object;
struct _Op_Remove_Object
{
  Scene* s;
  Eina_List* objects;
};

void operation_remove_object_do(Control *c, void* data);
void operation_remove_object_undo(Control *c, void* data);
void control_remove_object(Control* c, Scene* s, Eina_List* objects);


/////////////////////////////////////////////////////
typedef struct _Op_Change_Property Op_Change_Property;
struct _Op_Change_Property
{
  Object* o;
  Property* p;
  void* value_old;
  void* value_new;
};

void operation_change_property_do(Control *c, void* data);
void operation_change_property_undo(Control *c, void* data);
void control_change_property(Control* c, Object* o, Property* p, void* data);

#endif
