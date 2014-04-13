#ifndef __property__
#define __property__
#include <Eina.h>
#include <Eet.h>
#include "stdbool.h"

typedef struct _Property Property;

typedef enum _Hint Hint;
enum _Hint{
  VERTICAL,
  HORIZONTAL
};

typedef enum _ResourceType ResourceType;
enum _ResourceType{
  RESOURCE_TEXTURE,
  RESOURCE_MESH,
  RESOURCE_SHADER,
  RESOURCE_SCENE,
  RESOURCE_PREFAB,
  RESOURCE_ARMATURE
};

struct _Property
{
  const char* name;
  int type;
  int offset;
  size_t size;
  Property* sub;

  ResourceType resource_type;

  Eina_List* list;
  Hint hint;
  Eet_Data_Descriptor *descriptor;
  bool hide_name;
};

int property_offset_get(const Property* p);

Property* property_set_new();
int property_type_check(int type);
Property* property_real_parent_get(Property* p);

enum {
  PROPERTY_FILENAME = EET_I_LIMIT, //128
  PROPERTY_STRUCT,
  PROPERTY_STRUCT_NESTED,
  PROPERTY_POINTER,
  PROPERTY_RESOURCE,
  PROPERTY_ROOT,
  PROPERTY_UNIFORM,
  PROPERTY_QUAT,
  PROPERTY_OBJECT,
  TEST1,
  TEST2
};

#define PROPERTY_SET_TYPE(ps, type) \
 if (!ps->descriptor) { \
   Eet_Data_Descriptor_Class eddc; \
   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, type); \
   ps->descriptor = eet_data_descriptor_stream_new(&eddc); \
   ps->name = # type; \
 }

#define PROPERTY_NEW(p, struct_type, member, member_type) \
 do { \
   struct_type ___ett; \
   p = calloc(1, sizeof *p); \
   p->name = # member; \
   p->type = member_type; \
   p->offset = (char *)(& (___ett.member)) - (char *)(& (___ett)); \
   p->size = sizeof ___ett.member; \
   \
 } while(0)


#define PROPERTY_BASIC_ADD_WITH_NAME(ps, struct_type, member, member_type, name_str) \
 do { \
   Property *p; \
   PROPERTY_NEW(p, struct_type, member, member_type); \
   p->name = name_str; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_BASIC(ps->descriptor, struct_type, name_str, member, member_type); \
 } while(0)

#define PROPERTY_BASIC_ADD(ps, struct_type, member, member_type) \
 PROPERTY_BASIC_ADD_WITH_NAME(ps, struct_type, member, member_type, # member)


#define PROPERTY_SUB_ADD(ps, struct_type, member, ssub) \
 do { \
   Property *p; \
   PROPERTY_NEW(p, struct_type, member, PROPERTY_STRUCT); \
   p->sub = ssub; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_SUB(ps->descriptor, struct_type, # member, member, ssub->descriptor); \
 } while(0)


#define PROPERTY_SUB_NESTED_ADD(ps, struct_type, member, ssub) \
 do { \
   Property *p; \
   PROPERTY_NEW(p, struct_type, member, PROPERTY_STRUCT_NESTED); \
   p->sub = ssub; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_SUB_NESTED(ps->descriptor, struct_type, # member, member, ssub->descriptor); \
 } while(0)

#define PROPERTY_HASH_ADD(ps, struct_type, member, psdata) \
 do { \
   Property* p; \
   PROPERTY_NEW(p, struct_type, member, EET_G_HASH); \
   p->sub = psdata; \
   ps->list = eina_list_append(ps->list, p); \
   PROPERTY_SET_TYPE(ps, struct_type); \
   EET_DATA_DESCRIPTOR_ADD_HASH(ps->descriptor, struct_type, # member, member, psdata->descriptor);\
 } while(0)

#ifndef EET_DATA_DESCRIPTOR_ADD_MAPPING_BASIC
#define EET_DATA_DESCRIPTOR_ADD_MAPPING_BASIC(unified_type, name, basic_type) \
  eet_data_descriptor_element_add(unified_type,                      \
                                  name,                              \
                                  basic_type,                        \
                                  EET_G_UNKNOWN,                     \
                                  0,                                 \
                                  0,                                 \
                                  NULL,                              \
                                  NULL)
#endif


Property* property_set_vec3();
Property* property_set_vec4();
Property* property_set_quat();
Property* property_set_resource_handle(ResourceType type);

#endif
