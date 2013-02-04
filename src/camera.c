#include "camera.h"

Camera*
create_camera()
{
  Camera* c = malloc(sizeof *c);
  return c;
}
