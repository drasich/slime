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

#define ADD_PROP(array, struct_type, member, member_type) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # member, member_type, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett)),                \
      sizeof ___ett.member};                \
    eina_inarray_push(array, &p); \
    printf("type %s, add_prop : %s,  %zu\n", # struct_type, # member, p.size);\
  } while(0)

#define ADD_PROP_NAME(array, struct_type, member, member_type, name) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # name, member_type, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett)),                \
      sizeof ___ett.member};                \
    eina_inarray_push(array, &p); \
    printf("type %s, add_prop name : %s,  %zu\n", # struct_type, # member, p.size);\
  } while(0)

#define ADD_PROP_ARRAY(array, struct_type, member, prop_array) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # member, PROPERTY_STRUCT, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett)),                \
      sizeof ___ett.member, \
      prop_array};                \
    add_offset(prop_array, p.offset); \
    eina_inarray_push(array, &p); \
    printf("type %s, add_prop array : %s,  %zu\n", # struct_type, # member, p.size);\
  } while(0)


PropertySet* create_property_set();

enum {
  PROPERTY_FILENAME = EET_I_LIMIT,
  PROPERTY_STRUCT,
  PROPERTY_POINTER,
  TEST1,
  TEST2
};

#define PROPERTY_SET_TYPE(ps, type) \
  Eet_Data_Descriptor_Class eddc; \
  EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, type); \
  ps->descriptor = eet_data_descriptor_file_new(&eddc);


#define PS_ADD_PROP_NAME(ps, struct_type, member, member_type, name) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # name, member_type, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett)),                \
      sizeof ___ett.member};                \
    eina_inarray_push(ps->array, &p); \
    printf("type %s, add_prop name : %s,  %zu\n", # struct_type, # member, p.size);\
    \
  EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, struct_type, # member, member, member_type);\
  } while(0)



#endif
