#ifndef __property__
#define __property__
#include <Eina.h>
#include <Eet.h>

typedef struct _Property Property;
typedef struct _PropertySet PropertySet;

struct _Property
{
  const char* name;
  int type;
  int offset;
  size_t size;
  PropertySet* array;
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


/*
#define ADD_PROP(array, struct_type, member, member_type) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # member, member_type, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett))};                \
    eina_inarray_push(array, &p); \
  } while(0)
*/

void add_offset(PropertySet* ps, int offset);


/*
#define ADD_PROP(ps, struct_type, member, member_type) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # member, member_type, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett)),                \
      sizeof ___ett.member};                \
    eina_inarray_push(ps->array, &p); \
    printf("type %s, add_prop : %s,  %zu\n", # struct_type, # member, p.size);\
  } while(0)

#define ADD_PROP_NAME(ps, struct_type, member, member_type, name) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # name, member_type, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett)),                \
      sizeof ___ett.member};                \
    eina_inarray_push(ps->array, &p); \
    printf("type %s, add_prop name : %s,  %zu\n", # struct_type, # member, p.size);\
  } while(0)

#define ADD_PROP_ARRAY(ps, struct_type, member, prop_array) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # member, PROPERTY_STRUCT, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett)),                \
      sizeof ___ett.member, \
      prop_array};                \
    add_offset(prop_array, p.offset); \
    eina_inarray_push(ps->array, &p); \
    printf("type %s, add_prop array : %s,  %zu\n", # struct_type, # member, p.size);\
  } while(0)
  */


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
   ps->descriptor = eet_data_descriptor_file_new(&eddc); \
 }


#define ADD_PROP_NAME(ps, struct_type, member, member_type, name) \
 do {                                                                      \
   struct_type ___ett;                                                  \
   Property p = { name, member_type, \
     (char *)(& (___ett.member)) -        \
     (char *)(& (___ett)),                \
     sizeof ___ett.member};                \
   eina_inarray_push(ps->array, &p); \
   printf("type %s, add_prop_name : %s,  %zu\n", # struct_type, # member, p.size);\
   \
   int mt = property_type_check(member_type);\
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, struct_type, # name, member, mt);\
 } while(0)

#define ADD_PROP(ps, struct_type, member, member_type) \
 ADD_PROP_NAME(ps, struct_type, member, member_type, # member)


#define ADD_PROP_ARRAY(ps, struct_type, member, prop_array) \
 do {                                                                      \
   struct_type ___ett;                                                  \
   Property p = { # member, PROPERTY_STRUCT, \
     (char *)(& (___ett.member)) -        \
     (char *)(& (___ett)),                \
     sizeof ___ett.member, \
     prop_array};                \
   add_offset(prop_array, p.offset); \
   eina_inarray_push(ps->array, &p); \
   printf("type %s, add_prop array : %s,  %zu\n", # struct_type, # member, p.size);\
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_SUB_NESTED(ps->descriptor, struct_type, # member, member, prop_array->descriptor);\
 } while(0)



#endif
