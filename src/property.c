#include <Eina.h>
#include "property.h"
#include "vec.h"
#include "quat.h"
#include "resource.h"

Property* property_set_new()
{
  Property *ps = calloc(1, sizeof *ps);
  ps->type = PROPERTY_ROOT;
  return ps;
}

int
property_offset_get(const Property* p)
{
  /*
  if (p->parent &&
        p->parent->type != PROPERTY_STRUCT &&
        p->parent->type != EET_G_HASH) {
    return p->offset + property_offset_get(p->parent);
  }
  else */{
    return p->offset;
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
  Property* ps = property_set_new();
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
  Property* ps = property_set_new();
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
  Property* ps = property_set_new();
  PROPERTY_SET_TYPE(ps, Quat);
  ps->hint = HORIZONTAL;
  ps->name = "quat";

  PROPERTY_BASIC_ADD(ps, Quat, x, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Quat, y, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Quat, z, EET_T_DOUBLE);
  PROPERTY_BASIC_ADD(ps, Quat, w, EET_T_DOUBLE);

  return ps;
}

Property* property_set_resource_handle(ResourceType type)
{
  Property* ps = property_set_new();
  PROPERTY_SET_TYPE(ps, ResourceHandle);
  ps->name = "resource";
  ps->type = PROPERTY_RESOURCE;
  ps->resource_type = type;

  EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, ResourceHandle, "name", name, EET_T_STRING);

  //PROPERTY_BASIC_ADD(ps, ResourceHandle, name, EET_T_STRING);

  return ps;
}

