#include <Eina.h>
#include "property.h"
#include "vec.h"

PropertySet* create_property_set()
{
  PropertySet *ps = calloc(1, sizeof *ps);

  ps->array = eina_inarray_new(sizeof(Property), 0);
  return ps;
}

void add_offset(PropertySet* ps, int offset)
{
  Property *p;
  EINA_INARRAY_FOREACH(ps->array, p) {
    p->offset += offset;
    if (p->array) add_offset(p->array, offset);
  }

}

int property_type_check(int type)
{
  if (type == PROPERTY_FILENAME)
  return EET_T_STRING;

  return type;
}

PropertySet*
property_set_vec3()
{
  PropertySet* ps = create_property_set();
  PROPERTY_SET_TYPE(ps, Vec3);
  ps->hint = HORIZONTAL;

  ADD_PROP_NAME(ps, Vec3, X, EET_T_DOUBLE, "x");
  ADD_PROP_NAME(ps, Vec3, Y, EET_T_DOUBLE, "y");
  ADD_PROP_NAME(ps, Vec3, Z, EET_T_DOUBLE, "z");

  return ps;
}

