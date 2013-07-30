#include "component/transform.h"
#include "property.h"
#include <Eet.h>

static const char VEC3_FILE_ENTRY[] = "Vec3";

static Eet_Data_Descriptor *_vec3_descriptor;
static Eet_Data_Descriptor *_transform_descriptor;


static const char TRANSFORM_FILE_ENTRY[] = "Transform";


static void
_descriptor_init(void)
{
  Eet_Data_Descriptor_Class eddc;

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Vec3);
  _vec3_descriptor = eet_data_descriptor_stream_new(&eddc);

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Transform);
  _transform_descriptor = eet_data_descriptor_stream_new(&eddc);

  /*
#define ADD_BASIC(member, eet_type) \
  EET_DATA_DESCRIPTOR_ADD_BASIC             \
  (_vec3_descriptor, Vec3, # member, member, eet_type)

  ADD_BASIC(X, EET_T_DOUBLE);
  ADD_BASIC(Y, EET_T_DOUBLE);
  ADD_BASIC(Z, EET_T_DOUBLE);

#undef ADD_BASIC

#define ADD_SUB(member, sub_type) \
  EET_DATA_DESCRIPTOR_ADD_SUB             \
  (_transform_descriptor, Transform, # member, member, sub_type)

  ADD_SUB(position, _vec3_descriptor);
  ADD_SUB(angles, _vec3_descriptor);

#undef ADD_SUB
o*/

#define ADD_BASIC(member, eet_type) \
  EET_DATA_DESCRIPTOR_ADD_BASIC             \
  (_transform_descriptor, Transform, # member, member, eet_type)
  ADD_BASIC(test, EET_T_DOUBLE);
  ADD_BASIC(position.X, EET_T_DOUBLE);
  ADD_BASIC(position.Y, EET_T_DOUBLE);
  ADD_BASIC(position.Z, EET_T_DOUBLE);
  ADD_BASIC(angles.X, EET_T_DOUBLE);
  ADD_BASIC(angles.Y, EET_T_DOUBLE);
  ADD_BASIC(angles.Z, EET_T_DOUBLE);
#undef ADD_BASIC
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
  Transform* t = calloc(1, sizeof *t);
  t->position = vec3(1,2,3);
  t->angles = vec3(4,5,6);
  t->test = 9.8765;
  return t;
}

static Transform*
test_load(const char *filename)
{
  Transform *t;
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_READ);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", filename);
    return NULL;
  }

  t = eet_data_read(ef, _transform_descriptor, TRANSFORM_FILE_ENTRY);
  eet_close(ef);
 
  return t;
}

static Eina_Bool
test_save(
      const Transform *t,
      const char *filename)
{
  //char tmp[256];
  Eina_Bool ret;
  Eet_File *ef = eet_open(filename, EET_FILE_MODE_WRITE);
  if (!ef) {
    fprintf(stderr, "error reading file %s \n", filename);
    return EINA_FALSE;
  }

  ret = eet_data_write(ef, _transform_descriptor, TRANSFORM_FILE_ENTRY, t, EINA_TRUE);
  eet_close(ef);
  if (ret) {
    printf("return value for save looks ok \n");
  }
  else
    printf("return value for save NOT OK \n");

  return ret;
}

void test_test()
{
  Transform* t;
  eet_init();
  _descriptor_init();
  t = test_load("test.eet");

  if (!t) {
    t = test_create();
    printf("I have to create\n");
  }

  printf(" position : %f, %f, %f \n", t->position.X, t->position.Y, t->position.Z);
  printf(" angles : %f, %f, %f \n", t->angles.X, t->angles.Y, t->angles.Z);
  printf(" test : %f \n", t->test);
  
  test_save(t, "test.eet");
  _descriptor_shutdown();
}

void test_display()
{
  Eina_Inarray * iarr = create_property_set();

  ADD_PROP(iarr, Transform, test, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, position.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, position.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, position.Z, EET_T_DOUBLE);

  Transform yep = { vec3(1.98,2,3), vec3(4,5,6), 7};
  printf("size of double %zu \n", sizeof(double));

  Property *p;
  EINA_INARRAY_FOREACH(iarr, p) {
   printf("name: %s , type: %d, offset: %d\n", p->name, p->type, p->offset);
   printf("   value is : ");
   switch(p->type) {
     case EET_T_DOUBLE:
        {
         double d;
         memcpy(&d, (void*)&yep + p->offset, sizeof d);
         printf("%f\n",d);
        }
         break;
     default:
       printf("novalue \n ");
   }
  }

  Transform* t = test_create();

  double test =  t->position.X;
}

static void *
_create_transform()
{
  Transform* t = calloc(1, sizeof *t);
  return t;
}

static Eina_Inarray* 
_transform_properties()
{
  Eina_Inarray * iarr = create_property_set();

  ADD_PROP(iarr, Transform, position.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, position.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, position.Z, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, angles.X, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, angles.Y, EET_T_DOUBLE);
  ADD_PROP(iarr, Transform, angles.Z, EET_T_DOUBLE);

  return iarr;
}


ComponentDesc transform_desc = {
  "transform",
  _create_transform,
  _transform_properties
};

