#include "resource_handle.h"

TextureHandle* texture_handle_new()
{
  TextureHandle* th = calloc(1, sizeof *th);
  return th;
}

