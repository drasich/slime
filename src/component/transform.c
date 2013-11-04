#include "component/transform.h"
#include "property.h"
#include <Eet.h>

static void *
_create_transform()
{
  Transform* t = calloc(1, sizeof *t);
  return t;
}

static Property* 
_transform_properties()
{
  Property* ps = property_set_new();

  //TODO clean the property sets
  Property *vec3 = property_set_vec3();
  PROPERTY_SUB_NESTED_ADD(ps, Transform, position, vec3);
  Property *an = property_set_vec3();
  PROPERTY_SUB_NESTED_ADD(ps, Transform, angles, an);

  return ps;
}


ComponentDesc transform_desc = {
  "transform",
  _create_transform,
  _transform_properties
};

