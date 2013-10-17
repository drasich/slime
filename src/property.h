#ifndef __property__
#define __property__
#include <Eina.h>
#include <Eet.h>
#include "stdbool.h"

typedef struct _Property Property;
typedef struct _PropertySet PropertySet;

struct _Property
{
  const char* name;
  int type;
  int offset;
  size_t size;
  PropertySet* array;

  bool is_resource;
  const char* resource_type;
};

typedef enum _Hint Hint;
enum _Hint{
  VERTICAL,
  HORIZONTAL
};

struct _PropertySet
{
  Eina_Inarray* array;
  Hint hint;
  Eet_Data_Descriptor *descriptor;
};

void add_offset(PropertySet* ps, int offset);

PropertySet* create_property_set();
int property_type_check(int type);

enum {
  PROPERTY_FILENAME = EET_I_LIMIT,
  PROPERTY_STRUCT,
  PROPERTY_POINTER,
  TEST1,
  TEST2
};

#define PROPERTY_SET_TYPE(ps, type) \
 if (!ps->descriptor) { \
   Eet_Data_Descriptor_Class eddc; \
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, type); \
   ps->descriptor = eet_data_descriptor_stream_new(&eddc); \
 }


#define ADD_PROP_NAME(ps, struct_type, member, member_type, name) \
 do {                                                                      \
   struct_type ___ett;                                                  \
   Property p = { name, member_type, \
     (char *)(& (___ett.member)) -        \
     (char *)(& (___ett)),                \
     sizeof ___ett.member};                \
   eina_inarray_push(ps->array, &p); \
   \
   int mt = property_type_check(member_type);\
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, struct_type, name, member, mt);\
 } while(0)

#define ADD_PROP(ps, struct_type, member, member_type) \
 ADD_PROP_NAME(ps, struct_type, member, member_type, # member)


#define ADD_PROP_STRUCT_NESTED(ps, struct_type, member, sub) \
 do {                                                                      \
   struct_type ___ett;                                                  \
   Property p = { # member, PROPERTY_STRUCT, \
     (char *)(& (___ett.member)) -        \
     (char *)(& (___ett)),                \
     sizeof ___ett.member, \
     sub};                \
   add_offset(sub, p.offset); \
   eina_inarray_push(ps->array, &p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_SUB_NESTED(ps->descriptor, struct_type, # member, member, sub->descriptor); \
 } while(0)

PropertySet* property_set_vec3();
PropertySet* property_set_vec4();
PropertySet* property_set_quat();

#define ADD_RESOURCE(ps, struct_type, member, resource_type) \
 do {                                                   \
   struct_type ___ett;                                                  \
   Property p = { resource_type, EET_T_STRING, \
     (char *)(& (___ett.member)) -        \
     (char *)(& (___ett)),                \
     sizeof ___ett.member, \
     NULL,\
     true,\
     resource_type\
     };                \
   eina_inarray_push(ps->array, &p); \
   \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, struct_type, # member, member, EET_T_STRING);\
 } while(0)


#endif
