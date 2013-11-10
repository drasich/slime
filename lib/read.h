#ifndef __read__
#define __read__
#include <stdio.h>
#include <stdint.h>
#include "vec.h"

char* read_name(FILE*f);
char* read_string(FILE*f);
Vec3 read_vec3(FILE* f);
Vec4 read_vec4(FILE* f);
uint16_t read_uint16(FILE* f);
float read_float(FILE* f);


#endif
