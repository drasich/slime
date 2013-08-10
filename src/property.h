#ifndef __property__
#define __property__
#include <Eina.h>
#include <Eet.h>

typedef struct _Property Property;

struct _Property
{
  const char* name;
  int type;
  int offset;
  size_t size;
  Eina_Inarray* array;
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

void add_offset(Eina_Inarray* a, int offset);

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


Eina_Inarray* create_property_set();

enum {
  PROPERTY_FILENAME = EET_I_LIMIT,
  PROPERTY_STRUCT,
  TEST1,
  TEST2
};

#endif
