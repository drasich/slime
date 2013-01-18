#include "read.h"
#include <stdlib.h>

char* read_string(FILE*f)
{
  uint16_t strlen;
  fread(&strlen, sizeof(strlen),1,f);
  char* name = malloc(strlen+1);
  fread(name, 1, strlen, f);
  name[strlen] = '\0';
  return name;
}

char* read_name(FILE* f)
{
  return read_string(f);
}

Vec3 read_vec3(FILE* f)
{
  float x,y,z;
  fread(&x, 4,1,f);
  fread(&y, 4,1,f);
  fread(&z, 4,1,f);
  Vec3 v = {x,y,z};
  return v;
}

Vec4 read_vec4(FILE* f)
{
  float x,y,z,w;
  fread(&x, 4,1,f);
  fread(&y, 4,1,f);
  fread(&z, 4,1,f);
  fread(&w, 4,1,f);
  Vec4 v = {x,y,z,w};
  return v;
}

uint16_t read_uint16(FILE* f)
{
  uint16_t count;
  fread(&count, sizeof(count),1,f);
  return count;
}

float read_float(FILE* f)
{
  float x;
  fread(&x, 4,1,f);
  return x;
}
