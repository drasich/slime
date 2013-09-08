#include "component/transform.h"
#include "property.h"
#include <Eet.h>

static void *
_create_transform()
{
  Transform* t = calloc(1, sizeof *t);
  return t;
}

static PropertySet* 
_transform_properties()
{
  PropertySet* ps = create_property_set();

  //TODO clean the property sets
  PropertySet *vec3 = property_set_vec3();
  ADD_PROP_STRUCT_NESTED(ps, Transform, position, vec3);
  PropertySet *an = property_set_vec3();
  ADD_PROP_STRUCT_NESTED(ps, Transform, angles, an);

  return ps;
}


ComponentDesc transform_desc = {
  "transform",
  _create_transform,
  _transform_properties
};

