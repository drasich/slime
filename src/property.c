#include <Eina.h>
#include "property.h"

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
