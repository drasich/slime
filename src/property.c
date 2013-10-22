#include <Eina.h>
#include "property.h"
#include "vec.h"
#include "quat.h"

Property* create_property_set()
{
  Property *ps = calloc(1, sizeof *ps);
  ps->type = PROPERTY_STRUCT_NESTED;
  return ps;
}

int
property_offset_get(const Property* p)
{
  if (p->parent &&
        p->parent->type != PROPERTY_STRUCT &&
        p->parent->type != EET_G_HASH) {
    return p->offset + property_offset_get(p->parent);
  }
  else {
    return p->offset;
  }
}

Property* property_real_parent_get(Property* p)
{
  if (p->parent) {
    if (p->parent->type == PROPERTY_STRUCT ||
          p->parent->type == PROPERTY_STRUCT_NESTED) {
      printf("parent is struct, %s\n", p->name);
      return property_real_parent_get(p->parent);
    }
    else {
      printf("return parent, parent type %s, %d\n", p->parent->name, p->parent->type);
    return p->parent;
    }
  }
  else {
    printf("return null, %s\n", p->name);
    return NULL;
  }
}

int property_type_check(int type)
{
  if (type == PROPERTY_FILENAME)
  return EET_T_STRING;

  return type;
}

Property*
property_set_vec3()
{
  Property* ps = create_property_set();
  PROPERTY_SET_TYPE(ps, Vec3);
  ps->hint = HORIZONTAL;
  ps->name = "vec3";

  PROPERTY_BASIC_ADD(ps, Vec3, x, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Vec3, y, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Vec3, z, EET_T_DOUBLE);

  return ps;
}

Property*
property_set_vec4()
{
  Property* ps = create_property_set();
  PROPERTY_SET_TYPE(ps, Vec4);
  ps->hint = HORIZONTAL;
  ps->name = "vec4";

  PROPERTY_BASIC_ADD(ps, Vec4, x, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Vec4, y, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Vec4, z, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Vec4, w, EET_T_DOUBLE);

  return ps;
}

Property*
property_set_quat()
{
  Property* ps = create_property_set();
  PROPERTY_SET_TYPE(ps, Quat);
  ps->hint = HORIZONTAL;
  ps->name = "quat";

  PROPERTY_BASIC_ADD(ps, Quat, x, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Quat, y, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Quat, z, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Quat, w, EET_T_DOUBLE);

  return ps;
}



