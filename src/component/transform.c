#include "component/transform.h"
//#include <Eina.h>
#include <Eet.h>

static const char VEC3_FILE_ENTRY[] = "vec3";

static Eet_Data_Descriptor *_vec3_descriptor;
static Eet_Data_Descriptor *_transform_descriptor;


static const char TRANSFORM_FILE_ENTRY[] = "transform";


static void
_descriptor_init(void)
{
  Eet_Data_Descriptor_Class eddc;

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Vec3);
  _vec3_descriptor = eet_data_descriptor_stream_new(&eddc);

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Transform);
  _transform_descriptor = eet_data_descriptor_stream_new(&eddc);

#define ADD_BASIC(member, eet_type) \
  EET_DATA_DESCRIPTOR_ADD_BASIC             \
  (_vec3_descriptor, Vec3, # member, member, eet_type)

  ADD_BASIC(X, EET_T_DOUBLE);
  ADD_BASIC(Y, EET_T_DOUBLE);
  ADD_BASIC(z, EET_T_DOUBLE);

#undef ADD_BASIC

#define ADD_SUB(member, sub_type) \
  EET_DATA_DESCRIPTOR_ADD_SUB             \
  (_vec3_descriptor, Vec3, # member, member, sub_type)

  ADD_BASIC(position, _vec3_descriptor);
  ADD_BASIC(angles, _vec3_descriptor);

#undef ADD_SUB
}

static void
_descriptor_shutdown(void)
{
  eet_data_descriptor_free(_vec3_descriptor);
  eet_data_descriptor_free(_transform_descriptor);
} 

static Transform*
test_create()
{
  Transform* t = calloc(1, sizeof t);
  t->position = vec3(1,2,3);
  t->angles = vec3(4,5,6);
  return t;
}

static Transform*
test_load(const char *filename)
{
  Transform *t;
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
  fprintf(stderr, "error reading file %s \n", filename);

  t = eet_data_read(ef, _transform_descriptor, TRANSFORM_FILE_ENTRY);

  prinf(" position : %f, %f, %f \n", t->position.X, t->position.Y, t->position.Z);
  prinf(" angles : %f, %f, %f \n", t->angles.X, t->angles.Y, t->angles.Z);
  
  return t;
}

static Eina_Bool
test_save(
      const Transform *t,
      const char *filename)
{
  char tmp[256];
  Eina_Bool ret;
  //Eet_File *ef = eet_open(
  //TODO
}


