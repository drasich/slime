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

#define ADD_PROP(array, struct_type, member, member_type) \
  do {                                                                      \
    struct_type ___ett;                                                  \
    Property p = { # member, member_type, \
      (char *)(& (___ett.member)) -        \
      (char *)(& (___ett)),                \
      sizeof ___ett.member};                \
    eina_inarray_push(array, &p); \
    printf("add_prop : %s,  %zu\n", # member, p.size);\
  } while(0)


Eina_Inarray* create_property_set();

enum {
  PROPERTY_FILENAME = EET_I_LIMIT,
  TEST1,
  TEST2
};

#endif
