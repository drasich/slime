#include "object_pointer.h"

Property*
property_set_object_pointer(const char* name)
{
  static Property* ps = NULL;
  if (ps) {
    return ps;
  }
  ps = property_set_new();
  PROPERTY_SET_TYPE(ps, ObjectPointer);
  ps->name = name;
  ps->type = PROPERTY_OBJECT;

  //Serialize the id with the descriptor only
  EET_DATA_DESCRIPTOR_ADD_BASIC(
        ps->descriptor,
        ObjectPointer, "id", id, EET_T_ULONG_LONG);

  return ps;
}

