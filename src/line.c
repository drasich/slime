#include "line.h"

Line* 
create_line()
{
  Line* l = calloc(1, sizeof l);
  //TODO name, shader
  l->vertices = eina_inarray_new(sizeof(Vec3), 2);
  return l;
}

void
line_add(Line* l, Vec3 p1, Vec3 p2)
{
  eina_inarray_push(l->vertices, &p1);
  eina_inarray_push(l->vertices, &p2);
  //TODO gl vertices
}

void
line_add_box(Line* line, AABox box, Repere r)
{
  //TODO
}
