#include <Eina.h>
#include "property.h"

Eina_Inarray* create_property_set()
{
  return eina_inarray_new(sizeof(Property), 0);
}

void add_offset(Eina_Inarray* a, int offset)
{
  Property *p;
  EINA_INARRAY_FOREACH(a, p) {
    p->offset += offset;
    if (p->array) add_offset(p->array, offset);
  }

}
