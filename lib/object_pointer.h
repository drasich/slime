#ifndef __object_pointer__
#define __object_pointer__
#include "object.h"
#include "property.h"

Property* property_set_object_pointer(const char* name);

typedef struct _ObjectPointer ObjectPointer;

struct _ObjectPointer {
  Object* object;
  unsigned long long id;
  //Scene* s;
};

#endif
