#include <Eina.h>
#include "property.h"

Eina_Inarray* create_property_set()
{
  return eina_inarray_new(sizeof(Property), 0);
}

